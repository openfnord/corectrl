// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
