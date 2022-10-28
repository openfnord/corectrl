// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
  /// @param fallbackIcon data of the fallback icon to be cached
  /// @returns true on success
  virtual bool tryOrCache(IProfile::Info &info,
                          std::vector<char> const &fallbackIcon) = 0;

  /// Adds an icon to the cache, overwriting any previous icon for the profile.
  /// If this fails or there is no icon data, it will try to use the fallback
  /// default icon.
  /// @param info info of the profile to cache the icon for. His iconURL will be
  /// updated on success
  /// @param iconData data of the icon to be cached
  /// @returns true on success
  virtual bool cache(IProfile::Info &info, std::vector<char> const &iconData) = 0;

  /// Syncs the cache icon when necessary.
  /// @param info info of the profile whose icon will be synced in the cache
  /// @return [success, updated] pair. Updated will be true when a new icon is
  /// cached.
  virtual std::pair<bool, bool> syncCache(IProfile::Info &info) = 0;

  /// Cleans the profile cached icon on the cache.
  virtual void clean(IProfile::Info &info) = 0;

  virtual ~IProfileIconCache() = default;
};
