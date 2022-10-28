// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofilestorage.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class IProfileParser;
class IProfileFileParser;
class IProfileIconCache;

class ProfileStorage : public IProfileStorage
{
 public:
  ProfileStorage(std::filesystem::path &&path,
                 std::unique_ptr<IProfileParser> &&profileParser,
                 std::unique_ptr<IProfileFileParser> &&profileFileParser,
                 std::unique_ptr<IProfileIconCache> &&iconCache) noexcept;

  void init(IProfile const &defaultProfile) override;

  std::vector<std::unique_ptr<IProfile>>
  profiles(IProfile const &baseProfile) override;
  bool load(IProfile &profile) override;
  bool save(IProfile &profile) override;
  bool loadFrom(IProfile &profile,
                std::filesystem::path const &path) const override;
  bool exportTo(IProfile const &profile,
                std::filesystem::path const &path) const override;
  bool update(IProfile const &profile, IProfile::Info &newInfo) override;
  void remove(IProfile::Info &info) override;

 private:
  void initProfilesDirectory() const;
  bool profilesDirectoryExist() const;

  bool loadProfileFromStorage(std::filesystem::path const &path,
                              IProfile &profile) const;
  bool loadProfileFrom(std::filesystem::path const &path,
                       IProfile &profile) const;

  std::filesystem::path const path_;
  std::unique_ptr<IProfileParser> profileParser_;
  std::unique_ptr<IProfileFileParser> profileFileParser_;
  std::unique_ptr<IProfileIconCache> iconCache_;
  std::string fileExtension_;
  std::string profileDataFileName_;
};
