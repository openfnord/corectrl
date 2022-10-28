// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "filecache.h"

#include "common/fileutils.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <exception>
#include <stdexcept>
#include <utility>

namespace fs = std::filesystem;

FileCache::FileCache(std::filesystem::path &&path) noexcept
: path_(std::move(path))
{
}

void FileCache::init()
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

std::optional<std::filesystem::path>
FileCache::add(std::filesystem::path const &path, std::string const &name)
{
  if (cacheDirectoryExist()) {
    if (Utils::File::isFilePathValid(path)) {
      auto target = path_ / name;
      if (path != target) {
        try {
          fs::copy_file(path, target, fs::copy_options::overwrite_existing);
          return {target};
        }
        catch (std::exception const &e) {
          LOG(ERROR) << e.what();
        }
      }
      else // file is on cache
        return {target};
    }
    else
      LOG(ERROR) << fmt::format(
          "Cannot add {} to cache. Invalid or missing file.", path.c_str());
  }

  return {};
}

std::optional<std::filesystem::path>
FileCache::add(std::vector<char> const &data, std::string const &name)
{
  if (cacheDirectoryExist()) {
    auto target = path_ / name;
    if (Utils::File::writeFile(target, data))
      return {target};
  }

  return {};
}

void FileCache::remove(std::string const &name)
{
  if (cacheDirectoryExist()) {
    auto target = path_ / name;
    try {
      fs::remove(target);
    }
    catch (std::exception const &e) {
      LOG(ERROR) << e.what();
    }
  }
}

std::optional<std::filesystem::path>
FileCache::get(std::string const &name,
               std::filesystem::path const &defaultPath) const
{
  if (cacheDirectoryExist()) {
    auto target = path_ / name;
    if (Utils::File::isFilePathValid(target))
      return {target};
    else
      LOG(WARNING) << fmt::format(
          "Cannot get {} from cache. Invalid or missing file.", target.c_str());
  }

  if (!defaultPath.empty())
    return {defaultPath};
  else
    return {};
}

std::filesystem::path FileCache::path() const
{
  return path_;
}

bool FileCache::cacheDirectoryExist() const
{
  if (Utils::File::isDirectoryPathValid(path_))
    return true;

  LOG(ERROR) << fmt::format("Missing or invalid cache directory {}",
                            path_.c_str());
  return false;
}
