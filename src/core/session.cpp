// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "session.h"

#include "helper/ihelpermonitor.h"
#include "iprofileapplicator.h"
#include "iprofilemanager.h"
#include "iprofileview.h"
#include "iprofileviewfactory.h"
#include <algorithm>
#include <iterator>
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

void Session::addManualProfileObserver(
    std::shared_ptr<ISession::ManualProfileObserver> observer)
{
  std::lock_guard<std::mutex> lock(manualProfileObserversMutex_);
  auto const it = std::find(manualProfileObservers_.begin(),
                            manualProfileObservers_.end(), observer);
  if (it == manualProfileObservers_.cend())
    manualProfileObservers_.emplace_back(std::move(observer));
}

void Session::removeManualProfileObserver(
    std::shared_ptr<ISession::ManualProfileObserver> observer)
{
  std::lock_guard<std::mutex> lock(manualProfileObserversMutex_);
  manualProfileObservers_.erase(std::remove(manualProfileObservers_.begin(),
                                            manualProfileObservers_.end(),
                                            observer),
                                manualProfileObservers_.end());
}

void Session::init(ISysModel const &model)
{
  profileManager_->init(model);

  populateProfileExeIndex();
  createProfileViews({}, {std::string(IProfile::Info::GlobalID)});
  profileApplicator_->apply(*pViews_.back());

  helperMonitor_->init();
  watchProfiles();
}

void Session::toggleManualProfile(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(pViewsMutex_);
  std::lock_guard<std::mutex> mLock(manualProfileMutex_);

  auto baseView = getBaseView(pViews_, manualProfile_);

  // remove profile view of the active manual profile
  if (manualProfile_.has_value()) {
    pViews_.pop_back();
    notifyManualProfileToggled(*manualProfile_, false);
  }

  // update manual profile state
  if (manualProfile_.has_value() && manualProfile_ == profileName)
    manualProfile_ = std::nullopt;
  else
    manualProfile_ = profileName;

  // create the profile view of the manual profile
  if (manualProfile_.has_value()) {
    createProfileViews(baseView, {*manualProfile_});
    notifyManualProfileToggled(*manualProfile_, true);
  }

  // apply active profile view
  profileApplicator_->apply(*pViews_.back());
}

IProfileManager &Session::profileManager() const
{
  return *profileManager_;
}

void Session::profileAdded(std::string const &profileName)
{
  auto profile = profileManager_->profile(profileName);
  if (profile.has_value() && profile->get().active() &&
      profile->get().info().exe != IProfile::Info::ManualID) {

    auto const &exe = profile->get().info().exe;
    std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

    if (profileExeIndex_.find(exe) == profileExeIndex_.cend()) {
      profileExeIndex_.emplace(exe, profileName);
      helperMonitor_->watchApp(exe);
    }
  }
}

void Session::profileRemoved(std::string const &profileName)
{
  bool isManual = false;
  {
    std::lock_guard<std::mutex> lock(manualProfileMutex_);
    if (manualProfile_ == profileName) {
      manualProfile_ = std::nullopt;
      isManual = true;
      notifyManualProfileToggled(profileName, false);
    }
  }

  if (!isManual) { // remove from profile executable index
    std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

    auto const profileIndexIter = std::find_if(
        profileExeIndex_.cbegin(), profileExeIndex_.cend(),
        [&](auto &indexItem) { return indexItem.second == profileName; });

    if (profileIndexIter != profileExeIndex_.cend()) {
      helperMonitor_->forgetApp(profileIndexIter->first);
      profileExeIndex_.erase(profileIndexIter);
    }
  }

  dequeueProfileView(profileName);
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
    {
      std::lock_guard<std::mutex> lock(manualProfileMutex_);
      createProfileViews(getBaseView(pViews_, manualProfile_), pViewsToRecreate);
    }

    // apply active profile view
    profileApplicator_->apply(*pViews_.back());
  }
}

void Session::profileActiveChanged(std::string const &profileName, bool active)
{
  auto profile = profileManager_->profile(profileName);
  if (profile.has_value() &&
      profile->get().info().exe != IProfile::Info::ManualID) {
    if (active)
      profileAdded(profileName);
    else
      profileRemoved(profileName);
  }
}

void Session::profileSaved(std::string const &)
{
}

void Session::profileInfoChanged(IProfile::Info const &oldInfo,
                                 IProfile::Info const &newInfo)
{
  if (oldInfo.exe != newInfo.exe || oldInfo.name != newInfo.name) {

    // sync profile executable index
    {
      std::lock_guard<std::mutex> lock(profileExeIndexMutex_);

      profileExeIndex_.erase(oldInfo.exe);
      if (newInfo.exe != IProfile::Info::ManualID)
        profileExeIndex_.emplace(newInfo.exe, newInfo.name);
    }

    // update monitor
    if (oldInfo.exe != newInfo.exe) {
      if (oldInfo.exe != IProfile::Info::ManualID)
        helperMonitor_->forgetApp(oldInfo.exe);

      if (newInfo.exe != IProfile::Info::ManualID)
        helperMonitor_->watchApp(newInfo.exe);
    }

    // handle active manual profile
    if (oldInfo.exe == IProfile::Info::ManualID) {
      std::lock_guard<std::mutex> lock(manualProfileMutex_);

      if (oldInfo.name == manualProfile_) {
        // the manual profile has been turned into an automatic profile
        if (newInfo.exe != IProfile::Info::ManualID)
          manualProfile_ = std::nullopt;
        else // only the profile name has changed
          manualProfile_ = newInfo.name;
      }
    }

    // handle profile view
    {
      std::lock_guard<std::mutex> lock(pViewsMutex_);

      // find the profile view
      auto profileViewIter = std::find_if(
          pViews_.cbegin(), pViews_.cend(),
          [&](auto &pv) { return pv->name() == oldInfo.name; });

      if (profileViewIter != pViews_.end()) {
        std::vector<std::string> pViewsToRecreate;
        pViewsToRecreate.reserve(pViews_.size());

        // recreate the profile view when only its name has been changed
        if (oldInfo.exe == newInfo.exe && oldInfo.name != newInfo.name)
          pViewsToRecreate.push_back(newInfo.name);

        // compute a list with the names of profile views to recreate
        auto nextProfileViewIter = std::next(profileViewIter);
        if (nextProfileViewIter != pViews_.end()) {
          std::transform(nextProfileViewIter, pViews_.cend(),
                         std::back_inserter(pViewsToRecreate),
                         [](auto &pv) { return pv->name(); });
        }

        // remove affected the profile views
        pViews_.erase(profileViewIter, pViews_.cend());

        // recreate the list of profile views
        {
          std::lock_guard<std::mutex> lock(manualProfileMutex_);
          createProfileViews(getBaseView(pViews_, manualProfile_),
                             pViewsToRecreate);
        }

        // apply active profile view
        profileApplicator_->apply(*pViews_.back());
      }
    }
  }
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

  queueProfileView(profileName);
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

  dequeueProfileView(profileName);
}

void Session::populateProfileExeIndex()
{
  auto profiles = profileManager_->profiles();
  for (auto &profileName : profiles) {
    auto profile = profileManager_->profile(profileName);
    auto info = profile->get().info();
    if (profile->get().active() && info.exe != IProfile::Info::ManualID)
      profileExeIndex_.emplace(info.exe, std::move(profileName));
  }
}

void Session::watchProfiles()
{
  for (auto const &[exe, name] : profileExeIndex_) {
    if (exe != IProfile::Info::GlobalID && exe != IProfile::Info::ManualID)
      helperMonitor_->watchApp(exe);
  }
}

void Session::createProfileViews(
    std::optional<std::reference_wrapper<IProfileView>> baseProfileView,
    std::vector<std::string> const &profileNames)
{
  for (auto &profileName : profileNames) {
    auto profile = profileManager_->profile(profileName);
    if (profile.has_value()) {
      auto profileView = profileViewFactory_->build(*profile, baseProfileView);
      pViews_.emplace_back(std::move(profileView));
    }
  }
}

std::optional<std::reference_wrapper<IProfileView>>
Session::getBaseView(std::deque<std::unique_ptr<IProfileView>> const &pViews,
                     std::optional<std::string> const &manualProfile) const
{
  std::optional<std::reference_wrapper<IProfileView>> base;

  if (!pViews.empty()) {
    if (!manualProfile.has_value())
      base = *pViews.back();
    else {
      auto baseIt = std::next(pViews.rbegin());
      if (baseIt != pViews.rend())
        base = **baseIt;
    }
  }

  return base;
}

void Session::queueProfileView(std::string const &profileName)
{
  // compute a list of profile views to create including the profile
  std::vector<std::string> pViewsToRecreate{profileName};

  std::lock_guard<std::mutex> lock(pViewsMutex_);
  std::lock_guard<std::mutex> mLock(manualProfileMutex_);

  auto baseView = getBaseView(pViews_, manualProfile_);

  // recreate active manual profile view
  if (manualProfile_.has_value()) {
    pViewsToRecreate.push_back(*manualProfile_);
    pViews_.pop_back();
  }

  createProfileViews(baseView, pViewsToRecreate);

  // apply active profile view
  profileApplicator_->apply(*pViews_.back());
}

void Session::dequeueProfileView(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(pViewsMutex_);

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
    {
      std::lock_guard<std::mutex> lock(manualProfileMutex_);
      createProfileViews(getBaseView(pViews_, manualProfile_), pViewsToRecreate);
    }

    // apply active profile view
    profileApplicator_->apply(*pViews_.back());
  }
}

void Session::notifyManualProfileToggled(std::string const &profileName,
                                         bool active)
{
  std::lock_guard<std::mutex> lock(manualProfileObserversMutex_);
  for (auto &o : manualProfileObservers_)
    o->toggled(profileName, active);
}
