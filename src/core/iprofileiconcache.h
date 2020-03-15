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

#include "iprofile.h"
#include <filesystem>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

class IProfileIconCache
{
 public:
  /// Initialization method.
  virtual void init() = 0;

  /// Tries to use an icon from the cache, caching the fallback icon when no icon
  /// was cached. If this also fails, it will try to use the fallback default icon.
  /// @param info info of the profile to cache the icon for. His iconURL will be
  /// updated on success
  /// @param fallbackIconReader data of the fallback icon to be cached
  /// @returns true on success
  virtual bool tryOrCache(
      IProfile::Info &info,
      std::function<std::optional<std::vector<char>>()> &&fallbackIconReader) = 0;

  /// Adds an icon to the cache, overwriting any previous icon for the profile.
  /// If this fails or there is no icon data, it will try to use the fallback
  /// default icon.
  /// @param info info of the profile to cache the icon for. His iconURL will be
  /// updated on success
  /// @param iconData data of the icon to be cached
  /// @returns true on success
  virtual bool cache(IProfile::Info &info,
                     std::optional<std::vector<char>> const &iconData) = 0;

  /// Synces the cache icon if it is necessary.
  /// @param info info of the profile for wich sync the icon on the cache
  /// @param fallbackIconReader data of the fallback icon to be cached
  /// @return [success, updated] pair. Updated will be true when a new icon is
  /// cached.
  virtual std::pair<bool, bool> syncCache(
      IProfile::Info &info,
      std::function<std::optional<std::vector<char>>()> &&fallbackIconReader) = 0;

  /// Cleans the profile cached icon on the cache.
  virtual void clean(IProfile::Info &info) = 0;

  virtual ~IProfileIconCache() = default;
};
