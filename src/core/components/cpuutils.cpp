// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
