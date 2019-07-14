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
