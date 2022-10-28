// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "common/fileutils.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class GPUInfoUeventDataSource
: public IDataSource<std::vector<std::string>, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "uevent";
  }

  bool read(std::vector<std::string> &data,
            std::filesystem::path const &path) override
  {
    auto const filePath = path / source();
    auto lines = Utils::File::readFileLines(filePath);
    if (!lines.empty()) {
      std::swap(data, lines);
      return true;
    }

    LOG(WARNING) << fmt::format("Cannot retrieve device information from {}",
                                filePath.c_str());
    return false;
  }
};
