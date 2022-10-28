// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofileiconcache.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

class IFileCache;

class ProfileIconCache final : public IProfileIconCache
{
 public:
  ProfileIconCache(std::unique_ptr<IFileCache> &&cache) noexcept;

  void init() override;

  bool tryOrCache(IProfile::Info &info,
                  std::vector<char> const &fallbackIcon) override;

  bool cache(IProfile::Info &info, std::vector<char> const &iconData) override;

  std::pair<bool, bool> syncCache(IProfile::Info &info) override;

  void clean(IProfile::Info &info) override;

 private:
  std::optional<std::filesystem::path>
  cacheIconFromData(std::vector<char> const &iconData,
                    IProfile::Info const &info) const;

  std::unique_ptr<IFileCache> const cache_;
};
