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
                  std::function<std::optional<std::vector<char>>()>
                      &&fallbackIconReader) override;

  bool cache(IProfile::Info &info,
             std::optional<std::vector<char>> const &iconData) override;

  std::pair<bool, bool> syncCache(IProfile::Info &info,
                                  std::function<std::optional<std::vector<char>>()>
                                      &&fallbackIconReader) override;

  void clean(IProfile::Info &info) override;

 private:
  std::optional<std::filesystem::path>
  cacheIconFromData(std::optional<std::vector<char>> const &iconData,
                    IProfile::Info const &info) const;

  std::unique_ptr<IFileCache> const cache_;
};
