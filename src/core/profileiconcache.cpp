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
#include "profileiconcache.h"

#include "common/fileutils.h"
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

bool ProfileIconCache::tryOrCache(
    IProfile::Info &info,
    std::function<std::optional<std::vector<char>>()> &&fallbackIconReader)
{
  // try the cache
  auto cacheURL = cache_->get(info.exe);
  if (cacheURL.has_value()) {
    if (info.iconURL != *cacheURL)
      info.iconURL = *cacheURL;
    return true;
  }

  // cache icon from fallback icon
  return cache(info, fallbackIconReader());
}

bool ProfileIconCache::cache(IProfile::Info &info,
                             std::optional<std::vector<char>> const &iconData)
{
  auto cacheURL = cacheIconFromData(iconData, info);
  if (cacheURL.has_value()) {
    info.iconURL = *cacheURL;
    return true;
  }

  return false;
}

std::pair<bool, bool> ProfileIconCache::syncCache(
    IProfile::Info &info,
    std::function<std::optional<std::vector<char>>()> &&fallbackIconReader)
{
  if (info.iconURL != IProfile::Info::GlobalIconURL &&
      info.iconURL != IProfile::Info::DefaultIconURL) {

    auto cacheURL = cache_->add(info.iconURL, info.exe);
    if (cacheURL.has_value()) {
      if (info.iconURL != *cacheURL)
        info.iconURL = *cacheURL;

      return {true, info.iconURL != *cacheURL};
    }
  }

  auto success = cache(info, fallbackIconReader());
  return {success, success};
}

void ProfileIconCache::clean(IProfile::Info &info)
{
  cache_->remove(info.exe);
  info.iconURL = IProfile::Info::DefaultIconURL;
}

std::optional<std::filesystem::path> ProfileIconCache::cacheIconFromData(
    std::optional<std::vector<char>> const &iconData,
    IProfile::Info const &info) const
{
  if (iconData.has_value()) {
    auto cacheURL = cache_->add(*iconData, info.exe);
    if (cacheURL.has_value())
      return cacheURL;
    else
      LOG(ERROR) << fmt::format("Failed to cache icon for {}", info.exe.data());
  }
  else { // no icon data!

    // use default or global icon
    auto url = info.iconURL;
    if (info.iconURL != IProfile::Info::GlobalIconURL &&
        info.iconURL != IProfile::Info::DefaultIconURL)
      url = IProfile::Info::DefaultIconURL;

    auto rccData = Utils::File::readQrcFile(url);
    if (!rccData.empty()) {
      auto cacheURL = cache_->add(rccData, info.exe);
      if (cacheURL.has_value())
        return cacheURL;
      else
        LOG(ERROR) << fmt::format("Failed to cache icon {}", url.data());
    }
    else
      LOG(ERROR) << fmt::format("Cannot read {} icon", url.data());
  }

  return {};
}
