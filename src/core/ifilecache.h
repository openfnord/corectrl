// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class IFileCache
{
 public:
  /// Initializes the file chache.
  virtual void init() = 0;

  /// Adds an existing file to the cache.
  /// @param path to the file to be cached
  /// @param name target name for the cached file
  /// @return path to the cached file when succeed
  virtual std::optional<std::filesystem::path>
  add(std::filesystem::path const &path, std::string const &name) = 0;

  /// Adds a new file containing data to the cache.
  /// @param data new file data to be cached
  /// @param name target name for the cached file
  /// @return path to the cached file when succeed
  virtual std::optional<std::filesystem::path> add(std::vector<char> const &data,
                                                   std::string const &name) = 0;

  /// Removes a cached file.
  /// @param name name of the cached file to remove
  virtual void remove(std::string const &name) = 0;

  /// Gets the path of a file in the cache.
  /// @param name name of the file
  /// @param defaultPath path to be returned if no file exists on the cache
  /// @return path to the file or defaultPath is set and no file with that name exists
  /// on the cache
  virtual std::optional<std::filesystem::path>
  get(std::string const &name,
      std::filesystem::path const &defaultPath = "") const = 0;

  /// Gets the path to the cache.
  virtual std::filesystem::path path() const = 0;

  virtual ~IFileCache() = default;
};
