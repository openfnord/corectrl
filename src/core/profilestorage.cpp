// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilestorage.h"

#include "common/fileutils.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "iprofilefileparser.h"
#include "iprofileiconcache.h"
#include "iprofileparser.h"
#include <exception>
#include <stdexcept>
#include <utility>

namespace fs = std::filesystem;

ProfileStorage::ProfileStorage(
    std::filesystem::path &&path,
    std::unique_ptr<IProfileParser> &&profileParser,
    std::unique_ptr<IProfileFileParser> &&profileFileParser,
    std::unique_ptr<IProfileIconCache> &&iconCache) noexcept
: path_(std::move(path))
, profileParser_(std::move(profileParser))
, profileFileParser_(std::move(profileFileParser))
, iconCache_(std::move(iconCache))
, profileDataFileName_(IProfileFileParser::ProfileDataFileName)
{
  fileExtension_ += "." + profileFileParser_->fileExtension();
  profileDataFileName_ += "." + profileParser_->format();
}

void ProfileStorage::init(IProfile const &defaultProfile)
{
  initProfilesDirectory();

  // initialize components
  defaultProfile.exportWith(*profileParser_->initializer());
  iconCache_->init();
}

std::vector<std::unique_ptr<IProfile>>
ProfileStorage::profiles(IProfile const &baseProfile)
{
  std::vector<std::unique_ptr<IProfile>> profiles;

  if (profilesDirectoryExist()) {
    for (auto &pathIt : fs::directory_iterator(path_)) {

      auto filePath = pathIt.path();
      if (Utils::File::isFilePathValid(filePath) &&
          filePath.extension() == fileExtension_) {

        auto profile = baseProfile.clone();
        if (loadProfileFromStorage(filePath, *profile))
          profiles.emplace_back(std::move(profile));
      }
    }
  }

  return profiles;
}

bool ProfileStorage::load(IProfile &profile)
{
  if (profilesDirectoryExist()) {
    auto info = profile.info();
    auto fileName = info.exe != IProfile::Info::ManualID
                        ? info.exe + fileExtension_
                        : info.exe + info.name + fileExtension_;
    return loadProfileFromStorage(path_ / fileName, profile);
  }

  return false;
}

bool ProfileStorage::save(IProfile &profile)
{
  if (profilesDirectoryExist()) {

    auto info = profile.info();
    auto fileName = info.exe != IProfile::Info::ManualID
                        ? info.exe + fileExtension_
                        : info.exe + info.name + fileExtension_;
    auto exported = exportTo(profile, path_ / fileName);
    if (exported) {

      if (info.hasCustomIcon()) {

        // sync icon cache when custom icons are used
        auto [success, updated] = iconCache_->syncCache(info);
        if (success && updated)
          profile.info(info);
      }

      return true;
    }
  }

  return false;
}

bool ProfileStorage::loadFrom(IProfile &profile,
                              std::filesystem::path const &path) const
{
  if (Utils::File::isFilePathValid(path) && path.extension() == fileExtension_)
    return loadProfileFrom(path, profile);
  else
    LOG(ERROR) << fmt::format("Cannot load {}. Invalid file.", path.c_str());

  return false;
}

bool ProfileStorage::exportTo(IProfile const &profile,
                              std::filesystem::path const &path) const
{
  std::vector<char> profileData;
  if (profileParser_->save(profileData, profile)) {

    std::vector<std::pair<std::string, std::vector<char>>> data;
    data.emplace_back(std::string(profileDataFileName_), std::move(profileData));

    auto info = profile.info();
    if (info.hasCustomIcon()) {

      // save the custom icon in the profile
      auto iconData = Utils::File::readFile(info.iconURL);
      if (!iconData.empty())
        data.emplace_back(std::string(IProfileFileParser::IconDataFileName),
                          std::move(iconData));
    }

    auto targetFilePath = path;
    if (targetFilePath.extension() != fileExtension_)
      targetFilePath += fileExtension_;

    return profileFileParser_->save(targetFilePath, data);
  }

  return false;
}

bool ProfileStorage::update(IProfile const &profile, IProfile::Info &newInfo)
{
  auto updatedProfile = profile.clone();
  if (!load(*updatedProfile))
    return false;

  // manual profiles are always active
  if (newInfo.exe == IProfile::Info::ManualID && !updatedProfile->active())
    updatedProfile->activate(true);

  auto oldInfo = profile.info();
  updatedProfile->info(newInfo);
  if (!save(*updatedProfile))
    return false;

  auto cacheURL = updatedProfile->info().iconURL;
  if (cacheURL != newInfo.iconURL)
    newInfo.iconURL = cacheURL;

  if (oldInfo.exe != newInfo.exe ||
      (oldInfo.exe == IProfile::Info::ManualID &&
       newInfo.exe == IProfile::Info::ManualID && oldInfo.name != newInfo.name))
    remove(oldInfo);

  return true;
}

void ProfileStorage::remove(IProfile::Info &info)
{
  if (profilesDirectoryExist()) {
    iconCache_->clean(info);

    auto fileName = info.exe != IProfile::Info::ManualID
                        ? info.exe + fileExtension_
                        : info.exe + info.name + fileExtension_;
    try {
      fs::remove(path_ / fileName);
    }
    catch (std::exception const &e) {
      LOG(ERROR) << e.what();
    }
  }
}

void ProfileStorage::initProfilesDirectory() const
{
  if (!fs::exists(path_)) {
    fs::create_directories(path_);
    fs::permissions(path_, fs::perms::owner_all | fs::perms::group_read |
                               fs::perms::group_exec | fs::perms::others_read |
                               fs::perms::others_exec);
  }

  if (!fs::is_directory(path_))
    throw std::runtime_error(
        fmt::format("{} is not a directory.", path_.c_str()));
}

bool ProfileStorage::profilesDirectoryExist() const
{
  if (Utils::File::isDirectoryPathValid(path_))
    return true;

  LOG(ERROR) << fmt::format(
      "Something went wrong with the profile storage directory: ", path_.c_str());

  return false;
}

bool ProfileStorage::loadProfileFromStorage(std::filesystem::path const &path,
                                            IProfile &profile) const
{
  auto profileData = profileFileParser_->load(path, profileDataFileName_);
  if (profileData.has_value()) {
    if (profileParser_->load(*profileData, profile)) {

      auto info = profile.info();
      if (info.exe == IProfile::Info::GlobalID)
        info.iconURL = IProfile::Info::GlobalIconURL;
      else {
        // try to read the icon stored in the profile
        auto profileIcon = profileFileParser_->load(
            path, std::string(IProfileFileParser::IconDataFileName));

        // profiles without icons use the default icon
        if (!profileIcon.has_value())
          info.iconURL = IProfile::Info::DefaultIconURL;

        // profiles with icons use a cached icon
        else if (iconCache_->tryOrCache(info, *profileIcon))
          profile.info(info);
      }

      return true;
    }
  }

  return false;
}

bool ProfileStorage::loadProfileFrom(std::filesystem::path const &path,
                                     IProfile &profile) const
{
  auto profileData = profileFileParser_->load(path, profileDataFileName_);
  if (profileData.has_value())
    return profileParser_->load(*profileData, profile);

  return false;
}
