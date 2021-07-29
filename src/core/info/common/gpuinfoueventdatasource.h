//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
