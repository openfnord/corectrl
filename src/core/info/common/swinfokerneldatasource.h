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
#include <string>

class SWInfoKernelDataSource : public IDataSource<std::string>
{
 public:
  std::string source() const override
  {
    return "/proc/version";
  }

  bool read(std::string &data) override
  {
    auto const lines = Utils::File::readFileLines(source());
    if (!lines.empty()) {
      data = lines.front();
      return true;
    }

    LOG(WARNING) << "Cannot retrieve kernel version";
    return false;
  }
};
