//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
#include "session.h"

#include "helper/ihelpermonitor.h"
#include "iprofileapplicator.h"
#include "iprofilemanager.h"
#include "iprofileview.h"
#include "iprofileviewfactory.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <optional>
#include <utility>

class Session::ProfileManagerObserver : public IProfileManager::Observer
{
 public:
  ProfileManagerObserver(Session &outer) noexcept
  : outer_(outer)
  {
  }

  void profileAdded(std::string const &profileName) override;
  void profileRemoved(std::string const &profileName) override;
  void profileChanged(std::string const &profileName) override;
  void profileActiveChanged(std::string const &profileName, bool active) override;
  void profileSaved(std::string const &profileName) override;
  void profileInfoChanged(IProfile::Info const &oldInfo,
                          IProfile::Info const &newInfo) override;

 private:
  Session &outer_;
};

void Session::ProfileManagerObserver::profileAdded(std::string const &profileName)
{
  outer_.profileAdded(profileName);
}

void Session::ProfileManagerObserver::profileRemoved(std::string const &profileName)
{
  outer_.profileRemoved(profileName);
}

void Session::ProfileManagerObserver::profileChanged(std::string const &profileName)
{
  outer_.profileChanged(profileName);
}

void Session::ProfileManagerObserver::profileActiveChanged(
    std::string const &profileName, bool active)
{
  outer_.profileActiveChanged(profileName, active);
}

void Session::ProfileManagerObserver::profileSaved(std::string const &profileName)
{
  outer_.profileSaved(profileName);
}

void Session::ProfileManagerObserver::profileInfoChanged(
    IProfile::Info const &oldInfo, IProfile::Info const &newInfo)
{
  outer_.profileInfoChanged(oldInfo, newInfo);
}

class Session::HelperMonitorObserver : public IHelperMonitor::Observer
{
 public:
  HelperMonitorObserver(Session &outer) noexcept
  : outer_(outer)
  {
  }

  void appExec(std::string appExe) override;
  void appExit(std::string appExe) override;

 private:
  Session &outer_;
};

void Session::HelperMonitorObserver::appExec(std::string appExe)
{
  outer_.queueProfileViewForExecutable(appExe);
}

void Session::HelperMonitorObserver::appExit(std::string appExe)
{
  outer_.dequeueProfileViewForExecutable(appExe);
}

Session::Session(std::shared_ptr<IProfileApplicator> profileApplicator,
                 std::unique_ptr<IProfileManager> &&profileManager,
                 std::unique_ptr<IProfileViewFactory> &&profileViewFactory,
                 std::unique_ptr<IHelperMonitor> &&helperMonitor) noexcept
: profileApplicator_(std::move(profileApplicator))
, profileManager_(std::move(profileManager))
, profileViewFactory_(std::move(profileViewFactory))
, helperMonitor_(std::move(helperMonitor))
, profileManagerObserver_(
      std::make_shared<Session::ProfileManagerObserver>(*this))
, helperMonitorObserver_(std::make_shared<Session::HelperMonitorObserver>(*this))
{
  profileManager_->addObserver(profileManagerObserver_);
  helperMonitor_->addObserver(helperMonitorObserver_);
}

void Session::init(ISysModel const &model)
{
  profileManager_->init(model);

  populateProfileIndex();
  createProfileViews({std::string(IProfile::Info::GlobalID)});
  profileApplicator_->apply(*pViews_.back());

  helperMonitor_->init();
  watchProfiles();
}

IProfileView const &Session::profileView() const
{
  return *pViews_.back();
}

IProfileManager &Session::profileManager() const
{
  return *profileManager_;
}

void Session::profileAdded(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

  if (profileExeIndex_.count(profileName) == 0) {
    auto profile = profileManager_->profile(profileName);
    if (profile.has_value() && profile->get().active()) {
      profileExeIndex_.emplace(profile->get().info().exe, profileName);
      helperMonitor_->watchApp(profile->get().info().exe);
    }
  }
}

void Session::profileRemoved(std::string const &profileName)
{
  // remove from profile index
  {
    std::lock_guard<std::mutex> lock(profileExeIndexMutex_);
    removeProfileIndexMapping(profileName);
  }

  {
    std::lock_guard<std::mutex> lock(pViewsMutex_);
    dequeueProfileView(profileName);
  }
}

void Session::profileChanged(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(pViewsMutex_);

  auto const profileViewIter = std::find_if(
      pViews_.cbegin(), pViews_.cend(),
      [&](auto &pv) { return pv->name() == profileName; });

  if (profileViewIter != pViews_.cend()) {
    // compute a list with the names of profile views to recreate
    std::vector<std::string> pViewsToRecreate;
    pViewsToRecreate.reserve(pViews_.size());
    std::transform(profileViewIter, pViews_.cend(),
                   std::back_inserter(pViewsToRecreate),
                   [](auto &pv) { return pv->name(); });

    // remove outdated profile views
    pViews_.erase(profileViewIter, pViews_.cend());

    // recreate the list of profile views
    createProfileViews(pViewsToRecreate);

    // apply active profile view
    profileApplicator_->apply(*pViews_.back());
  }
}

void Session::profileActiveChanged(std::string const &profileName, bool active)
{
  if (active)
    profileAdded(profileName);
  else
    profileRemoved(profileName);
}

void Session::profileSaved(std::string const &)
{
}

void Session::profileInfoChanged(IProfile::Info const &oldInfo,
                                 IProfile::Info const &newInfo)
{
  std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

  removeProfileIndexMapping(oldInfo.name);
  profileExeIndex_.emplace(newInfo.exe, newInfo.name);
}

void Session::queueProfileViewForExecutable(std::string const &executableName)
{
  std::string profileName;
  {
    std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

    auto const profileIndexNameIter = profileExeIndex_.find(executableName);
    if (profileIndexNameIter != profileExeIndex_.cend())
      profileName = profileIndexNameIter->second;
  }

  {
    std::lock_guard<std::mutex> lock(pViewsMutex_);
    queueProfileView(profileName);
  }
}

void Session::dequeueProfileViewForExecutable(std::string const &executableName)
{
  std::string profileName;
  {
    std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

    auto const profileIndexNameIter = profileExeIndex_.find(executableName);
    if (profileIndexNameIter != profileExeIndex_.cend())
      profileName = profileIndexNameIter->second;
  }

  {
    std::lock_guard<std::mutex> lock(pViewsMutex_);
    dequeueProfileView(profileName);
  }
}

void Session::populateProfileIndex()
{
  auto profiles = profileManager_->profiles();
  for (auto &profileName : profiles) {
    auto profile = profileManager_->profile(profileName);
    if (profile->get().active())
      profileExeIndex_.emplace(profile->get().info().exe, std::move(profileName));
  }
}

void Session::watchProfiles()
{
  auto profiles = profileManager_->profiles();
  for (auto &profileName : profiles) {
    auto profile = profileManager_->profile(profileName);
    auto &exe = profile->get().info().exe;
    if (exe != IProfile::Info::GlobalID)
      helperMonitor_->watchApp(exe);
  }
}

void Session::removeProfileIndexMapping(std::string const &profileName)
{
  auto const profileIndexIter = std::find_if(
      profileExeIndex_.cbegin(), profileExeIndex_.cend(),
      [&](auto &indexItem) { return indexItem.second == profileName; });
  if (profileIndexIter != profileExeIndex_.cend()) {
    helperMonitor_->forgetApp(profileIndexIter->first);
    profileExeIndex_.erase(profileIndexIter);
  }
}

void Session::createProfileViews(std::vector<std::string> const &profileNames)
{
  std::optional<std::reference_wrapper<IProfileView>> baseProfileView;
  if (!pViews_.empty())
    baseProfileView = *pViews_.back();

  for (auto &profileName : profileNames) {
    auto profile = profileManager_->profile(profileName);
    if (profile.has_value()) {
      auto profileView = profileViewFactory_->build(*profile, baseProfileView);
      pViews_.emplace_back(std::move(profileView));
    }
  }
}

void Session::queueProfileView(std::string const &profileName)
{
  // create the profile view for the profile
  createProfileViews({profileName});

  // apply active profile view
  profileApplicator_->apply(*pViews_.back());
}

void Session::dequeueProfileView(std::string const &profileName)
{
  auto profileViewIter = std::find_if(
      pViews_.cbegin(), pViews_.cend(),
      [&](auto &pv) { return pv->name() == profileName; });

  if (profileViewIter != pViews_.end()) {
    auto nextProfileView = std::next(profileViewIter);

    // compute a list with the names of profile views to recreate
    std::vector<std::string> pViewsToRecreate;
    if (nextProfileView != pViews_.end()) {
      pViewsToRecreate.reserve(pViews_.size());
      std::transform(nextProfileView, pViews_.cend(),
                     std::back_inserter(pViewsToRecreate),
                     [](auto &pv) { return pv->name(); });
    }

    // remove profile view and the outdated profile views
    pViews_.erase(profileViewIter, pViews_.cend());

    // recreate the list of profile views
    createProfileViews(pViewsToRecreate);

    // apply active profile view
    profileApplicator_->apply(*pViews_.back());
  }
}
