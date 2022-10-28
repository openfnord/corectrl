// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/idatasource.h"
#include <filesystem>
#include <string>
#include <vector>

class ZipDataSource final
: public IDataSource<std::string const, std::vector<char>>
{
 public:
  ZipDataSource(std::filesystem::path const &path) noexcept;

  std::string source() const override;
  bool read(std::string const &internalBundleDataPath,
            std::vector<char> &data) override;

 private:
  std::filesystem::path const path_;
};
