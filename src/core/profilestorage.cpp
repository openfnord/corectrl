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
    auto filePath = path_ / (profile.info().exe + fileExtension_);
    return loadProfileFromStorage(filePath, profile);
  }

  return false;
}

bool ProfileStorage::save(IProfile &profile)
{
  if (profilesDirectoryExist()) {
    auto filePath = path_ / (profile.info().exe + fileExtension_);

    std::vector<char> profileData;
    if (profileParser_->save(profileData, profile)) {
      auto info = profile.info();

      auto [success, updated] = iconCache_->syncCache(info, [&]() {
        return profileFileParser_->load(
            filePath, std::string(IProfileFileParser::IconDataFileName));
      });
      if (success && updated)
        profile.info(info);

      // pack data to be saved
      std::vector<std::pair<std::string, std::vector<char>>> data;
      data.emplace_back(std::string(profileDataFileName_),
                        std::move(profileData));

      auto iconData = Utils::File::readFile(info.iconURL);
      if (!iconData.empty())
        data.emplace_back(std::string(IProfileFileParser::IconDataFileName),
                          std::move(iconData));

      return profileFileParser_->save(filePath, data);
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

    auto profileFilePath = path_ / (profile.info().exe + fileExtension_);
    auto iconData = readIconData(profileFilePath, profile.info());
    if (iconData.has_value())
      data.emplace_back(std::string(IProfileFileParser::IconDataFileName),
                        std::move(iconData.value()));

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

  auto oldInfo = profile.info();
  updatedProfile->info(newInfo);
  if (!save(*updatedProfile))
    return false;

  auto cacheURL = updatedProfile->info().iconURL;
  if (cacheURL != newInfo.iconURL)
    newInfo.iconURL = cacheURL;

  if (oldInfo.exe != newInfo.exe)
    remove(oldInfo);

  return true;
}

void ProfileStorage::remove(IProfile::Info &info)
{
  if (profilesDirectoryExist()) {
    iconCache_->clean(info);

    auto fileName = info.exe + fileExtension_;
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

      if (iconCache_->tryOrCache(info, [&]() {
            return profileFileParser_->load(
                path, std::string(IProfileFileParser::IconDataFileName));
          })) {
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

std::optional<std::vector<char>>
ProfileStorage::readIconData(std::filesystem::path const &path,
                             IProfile::Info const &info) const
{
  // try the info icon
  if (Utils::File::isFilePathValid(info.iconURL)) {
    auto cacheData = Utils::File::readFile(info.iconURL);
    if (!cacheData.empty())
      return {cacheData};
  }

  // load from profile file
  auto profileFileIconData = profileFileParser_->load(
      path, std::string(IProfileFileParser::IconDataFileName));
  if (profileFileIconData.has_value())
    return profileFileIconData;

  // use default or global icon
  auto url = info.iconURL;
  if (info.iconURL != IProfile::Info::GlobalIconURL &&
      info.iconURL != IProfile::Info::DefaultIconURL)
    url = IProfile::Info::DefaultIconURL;

  auto rccData = Utils::File::readQrcFile(url);
  if (!rccData.empty())
    return {rccData};

  LOG(ERROR) << fmt::format("Not icon found for {}", info.exe.data());

  return {};
}
