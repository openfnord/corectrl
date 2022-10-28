// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profileiconcache.h"

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "ifilecache.h"
#include <utility>

ProfileIconCache::ProfileIconCache(std::unique_ptr<IFileCache> &&cache) noexcept
: cache_(std::move(cache))
{
}

void ProfileIconCache::init()
{
  cache_->init();
}

bool ProfileIconCache::tryOrCache(IProfile::Info &info,
                                  std::vector<char> const &fallbackIcon)
{
  auto fileName = info.exe != IProfile::Info::ManualID ? info.exe
                                                       : info.exe + info.name;
  // try the cache
  auto cacheURL = cache_->get(fileName);
  if (cacheURL.has_value()) {
    if (info.iconURL != *cacheURL)
      info.iconURL = *cacheURL;
    return true;
  }

  // cache fallback icon
  return cache(info, fallbackIcon);
}

bool ProfileIconCache::cache(IProfile::Info &info,
                             std::vector<char> const &iconData)
{
  auto cacheURL = cacheIconFromData(iconData, info);
  if (cacheURL.has_value()) {
    info.iconURL = *cacheURL;
    return true;
  }

  return false;
}

std::pair<bool, bool> ProfileIconCache::syncCache(IProfile::Info &info)
{
  auto fileName = info.exe != IProfile::Info::ManualID ? info.exe
                                                       : info.exe + info.name;
  auto cacheURL = cache_->add(info.iconURL, fileName);
  if (cacheURL.has_value()) {
    auto updateURL = info.iconURL != *cacheURL;
    if (updateURL)
      info.iconURL = *cacheURL;

    return {true, updateURL};
  }

  LOG(ERROR) << fmt::format("Failed to cache icon for {}", fileName.data());
  return {false, false};
}

void ProfileIconCache::clean(IProfile::Info &info)
{
  auto fileName = info.exe != IProfile::Info::ManualID ? info.exe
                                                       : info.exe + info.name;
  cache_->remove(fileName);
}

std::optional<std::filesystem::path>
ProfileIconCache::cacheIconFromData(std::vector<char> const &iconData,
                                    IProfile::Info const &info) const
{
  auto fileName = info.exe != IProfile::Info::ManualID ? info.exe
                                                       : info.exe + info.name;
  auto cacheURL = cache_->add(iconData, fileName);
  if (cacheURL.has_value())
    return cacheURL;

  LOG(ERROR) << fmt::format("Failed to cache icon for {}", fileName.data());
  return {};
}
