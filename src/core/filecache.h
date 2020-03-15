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

#include "ifilecache.h"
#include <filesystem>

class FileCache : public IFileCache
{
 public:
  FileCache(std::filesystem::path &&path) noexcept;

  void init() override;
  std::optional<std::filesystem::path> add(std::filesystem::path const &path,
                                           std::string const &name) override;
  std::optional<std::filesystem::path> add(std::vector<char> const &data,
                                           std::string const &name) override;
  void remove(std::string const &name) override;
  std::optional<std::filesystem::path>
  get(std::string const &name,
      std::filesystem::path const &defaultPath) const override;
  std::filesystem::path path() const override;

 private:
  bool cacheDirectoryExist() const;

  std::filesystem::path const path_;
};
