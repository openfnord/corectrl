// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilemanager.h"

#include "core/isysmodel.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "iprofile.h"
#include <algorithm>
#include <iterator>
#include <regex>
#include <utility>

ProfileManager::ProfileManager(
    std::unique_ptr<IProfile> &&defautlProfile,
    std::unique_ptr<IProfileStorage> &&profileStorage) noexcept
: defaultProfile_(std::move(defautlProfile))
, profileStorage_(std::move(profileStorage))
{
}

void ProfileManager::addObserver(std::shared_ptr<IProfileManager::Observer> observer)
{
  std::lock_guard<std::mutex> lock(obMutex_);

  auto const it = std::find(observers_.begin(), observers_.end(), observer);
  if (it == observers_.end())
    observers_.emplace_back(std::move(observer));
}

void ProfileManager::removeObserver(
    std::shared_ptr<IProfileManager::Observer> const &observer)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                   observers_.end());
}

void ProfileManager::init(ISysModel const &model)
{
  // initialize default profile
  model.exportWith(*defaultProfile_->initializer());

  profileStorage_->init(*defaultProfile_);

  std::regex const invalidName(R"(^\s+$)");
  std::regex const invalidExe(R"(\\\|\/|\||\x00|\*|`|;|:|'|")");
  std::unordered_set<std::string> exes;

  bool globalFound = false;

  auto profiles = profileStorage_->profiles(*defaultProfile_);
  for (auto &profile : profiles) {
    auto &info = profile->info();

    if (info.exe == IProfile::Info::GlobalID) {

      auto overrideInfo = info;
      overrideInfo.name = IProfile::Info::GlobalID;
      overrideInfo.iconURL = IProfile::Info::GlobalIconURL;
      profile->info(overrideInfo);

      globalFound = true;
    }

    if (!globalFound && info.name == IProfile::Info::GlobalID) {
      LOG(WARNING) << fmt::format("Ignoring profile with name: '{}', exe: '{}'",
                                  info.name, info.exe);
      LOG(WARNING) << fmt::format(
          "Used profile name ({}) is reserved for the Global Profile", info.name);
      continue;
    }

    if (!globalFound && info.exe == IProfile::Info::GlobalID) {
      LOG(WARNING) << fmt::format("Ignoring profile with name: '{}', exe: '{}'",
                                  info.name, info.exe);
      LOG(WARNING) << fmt::format("Used profile executable name ({}) is "
                                  "reserved for the Global Profile",
                                  info.exe);
      continue;
    }

    if (std::regex_search(info.name, invalidName)) {
      LOG(WARNING) << fmt::format("Ignoring profile with name: '{}', exe: '{}'",
                                  info.name, info.exe);
      LOG(WARNING) << fmt::format("Profile name ({}) has invalid characters",
                                  info.name);
      continue;
    }

    auto profileIt = profiles_.find(info.name);
    if (profileIt != profiles_.cend()) {
      LOG(WARNING) << fmt::format("Ignoring profile with name: '{}', exe: '{}'",
                                  info.name, info.exe);
      LOG(WARNING) << fmt::format(
          "There is another profile with the same name ({})", info.name);
      continue;
    }

    if (info.exe != IProfile::Info::ManualID) {
      if (std::regex_search(info.exe, invalidExe)) {
        LOG(WARNING) << fmt::format(
            "Ignoring profile with name: '{}', exe: '{}'", info.name, info.exe);
        LOG(WARNING) << fmt::format(
            "Profile executable name ({}) has invalid characters", info.exe);
        continue;
      }

      auto exeIt = exes.find(info.exe);
      if (exeIt != exes.cend()) {
        LOG(WARNING) << fmt::format(
            "Ignoring profile with name: '{}', exe: '{}'", info.name, info.exe);
        LOG(WARNING) << fmt::format(
            "There is another profile for the same executable ({})", info.exe);
        continue;
      }

      exes.insert(info.exe);
    }

    profiles_.emplace(info.name, std::move(profile));
  }

  if (!globalFound) {
    auto globalProfile = defaultProfile_->clone();
    globalProfile->info({std::string(IProfile::Info::GlobalID),
                         std::string(IProfile::Info::GlobalID),
                         std::string(IProfile::Info::GlobalIconURL)});

    profileStorage_->save(*globalProfile);
    profiles_.emplace(globalProfile->info().name, std::move(globalProfile));
  }
}

std::vector<std::string> ProfileManager::profiles() const
{
  std::vector<std::string> profiles;
  profiles.reserve(profiles_.size());
  std::transform(profiles_.cbegin(), profiles_.cend(),
                 std::back_inserter(profiles),
                 [](auto &kv) { return kv.first; });
  return profiles;
}

std::optional<std::reference_wrapper<IProfile const>>
ProfileManager::profile(std::string const &profileName) const
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend())
    return *profileIt->second;

  return {};
}

std::vector<std::string> ProfileManager::unsavedProfiles() const
{
  std::vector<std::string> profiles;
  profiles.reserve(unsavedProfiles_.size());
  std::transform(unsavedProfiles_.cbegin(), unsavedProfiles_.cend(),
                 std::back_inserter(profiles),
                 [](auto &profile) { return profile; });
  return profiles;
}

bool ProfileManager::unsaved(std::string const &profileName) const
{
  return unsavedProfiles_.find(profileName) != unsavedProfiles_.cend();
}

void ProfileManager::add(IProfile::Info const &info)
{
  auto const profileIt = profiles_.find(info.name);
  if (profileIt == profiles_.cend()) {
    auto newProfile = defaultProfile_->clone();
    newProfile->info(info);
    profileStorage_->save(*newProfile);
    profiles_.emplace(info.name, std::move(newProfile));

    notifyProfileAdded(info.name);
  }
}

void ProfileManager::clone(IProfile::Info const &cloneInfo,
                           std::string const &baseProfileName)
{
  auto const profileIt = profiles_.find(baseProfileName);
  if (profileIt != profiles_.cend()) {

    auto const clonedProfileIt = profiles_.find(cloneInfo.name);
    if (clonedProfileIt == profiles_.cend()) {
      auto clone = profileIt->second->clone();
      clone->info(cloneInfo);

      // manual profiles are always active
      if (cloneInfo.exe == IProfile::Info::ManualID)
        clone->activate(true);

      profileStorage_->save(*clone);
      profiles_.emplace(cloneInfo.name, std::move(clone));

      notifyProfileAdded(cloneInfo.name);
    }
  }
}

void ProfileManager::remove(std::string const &profileName)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    auto info = profileIt->second->info();
    profileStorage_->remove(info);
    profiles_.erase(profileIt);

    notifyProfileRemoved(profileName);
  }
}

void ProfileManager::activate(std::string const &profileName, bool active)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    auto &profile = profileIt->second;

    // update stored profile active state
    auto storedProfile = profile->clone();
    profileStorage_->load(*storedProfile);
    storedProfile->activate(active);
    profileStorage_->save(*storedProfile);

    // update profile active state
    profile->activate(active);

    notifyProfileActiveChanged(profileName, active);
  }
}

void ProfileManager::reset(std::string const &profileName)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    auto resetedProfile = defaultProfile_->clone();
    resetedProfile->info(profileIt->second->info());
    resetedProfile->activate(profileIt->second->active());
    profiles_.insert_or_assign(profileName, std::move(resetedProfile));
    unsavedProfiles_.insert(profileName);

    notifyProfileChanged(profileName);
  }
}

void ProfileManager::restore(std::string const &profileName)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    profileStorage_->load(*profileIt->second);
    unsavedProfiles_.erase(profileName);

    notifyProfileChanged(profileName);
  }
}

void ProfileManager::update(std::string const &profileName,
                            IProfile::Info const &newInfo)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    auto &profile = *profileIt->second;
    auto oldInfo = profile.info();

    auto info = newInfo;
    profileStorage_->update(profile, info);

    // update profile info
    profile.info(info);
    if (info.name != profileName) {
      // re-inserting element with the new name as key
      auto profileNodeHandler = profiles_.extract(profileIt);
      profileNodeHandler.key() = info.name;
      profiles_.insert(std::move(profileNodeHandler));

      // updating unsaved profile name
      auto unsavedIt = unsavedProfiles_.find(profileName);
      if (unsavedIt != unsavedProfiles_.cend()) {
        unsavedProfiles_.erase(unsavedIt);
        unsavedProfiles_.insert(info.name);
      }
    }
    notifyProfileInfoChanged(oldInfo, info);

    // manual profiles are always active
    if (info.exe == IProfile::Info::ManualID && !profile.active()) {
      profile.activate(true);
      notifyProfileActiveChanged(info.name, true);
    }
  }
}

void ProfileManager::update(std::string const &profileName,
                            Importable::Importer &importer)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    profileIt->second->importWith(importer);
    unsavedProfiles_.insert(profileName);

    notifyProfileChanged(profileName);
  }
}

bool ProfileManager::loadFrom(std::string const &profileName,
                              std::filesystem::path const &path)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    auto &profile = *profileIt->second;

    auto info = profile.info();
    auto active = profile.active();

    if (!profileStorage_->loadFrom(profile, path))
      return false;

    profile.activate(active);
    profile.info(info);

    unsavedProfiles_.insert(profileName);
    notifyProfileChanged(profileName);

    return true;
  }

  return false;
}

void ProfileManager::save(std::string const &profileName)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend()) {
    profileStorage_->save(*profileIt->second);
    unsavedProfiles_.erase(profileName);

    notifyProfileSaved(profileName);
  }
}

bool ProfileManager::exportTo(std::string const &profileName,
                              std::filesystem::path const &path)
{
  auto const profileIt = profiles_.find(profileName);
  if (profileIt != profiles_.cend())
    return profileStorage_->exportTo(*profileIt->second, path);

  return false;
}

void ProfileManager::notifyProfileAdded(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->profileAdded(profileName);
}

void ProfileManager::notifyProfileRemoved(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->profileRemoved(profileName);
}

void ProfileManager::notifyProfileChanged(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->profileChanged(profileName);
}

void ProfileManager::notifyProfileActiveChanged(std::string const &profileName,
                                                bool active)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->profileActiveChanged(profileName, active);
}

void ProfileManager::notifyProfileSaved(std::string const &profileName)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->profileSaved(profileName);
}

void ProfileManager::notifyProfileInfoChanged(IProfile::Info const &oldInfo,
                                              IProfile::Info const &newInfo)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->profileInfoChanged(oldInfo, newInfo);
}
