// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "hwiddatasource.h"

#include "common/fileutils.h"
#include <utility>

HWIDDataSource::HWIDDataSource(std::string const &path) noexcept
: path_(path)
{
}

std::string HWIDDataSource::source() const
{
  return path_;
}

bool HWIDDataSource::read(std::vector<char> &data)
{
  auto fileData = Utils::File::readFile(path_);
  if (!fileData.empty()) {
    std::swap(data, fileData);
    return true;
  }
  return false;
}
