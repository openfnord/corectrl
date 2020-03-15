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
#include "cpuutils.h"

#include "common/stringutils.h"
#include <algorithm>
#include <iterator>

namespace Utils {
namespace CPU {

std::optional<std::string>
parseProcCpuInfo(std::vector<std::string> const &procCpuInfoLines, int cpuId,
                 std::string_view target)
{
  static constexpr std::string_view cpuIdStr{"processor"};

  auto lineIt = std::find_if(
      procCpuInfoLines.cbegin(), procCpuInfoLines.cend(),
      [=](std::string const &line) {
        if (line.find(cpuIdStr) != std::string::npos) {
          int value;
          auto idPos = line.find_first_not_of("\t: ", cpuIdStr.size());
          return idPos != std::string::npos &&
                 Utils::String::toNumber<int>(value, line.substr(idPos)) &&
                 value == cpuId;
        }
        return false;
      });

  if (lineIt != procCpuInfoLines.cend()) {
    lineIt = std::next(lineIt);

    for (; lineIt != procCpuInfoLines.cend(); lineIt = std::next(lineIt)) {

      if (lineIt->empty())
        break; // target not found on this processor section

      auto targetPos = lineIt->find(target);
      if (targetPos != std::string::npos) {
        auto dataPos = lineIt->find_first_not_of("\t: ", target.size());
        if (dataPos != std::string::npos)
          return lineIt->substr(dataPos);
      }
    }
  }

  return {};
}

} // namespace CPU
} // namespace Utils
