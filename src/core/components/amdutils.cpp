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
#include "amdutils.h"

#include "common/stringutils.h"
#include "units/units.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <regex>
#include <unordered_map>

namespace Utils {
namespace AMD {

bool readAMDGPUVRamSize(int deviceFD, units::data::megabyte_t *size)
{
#if defined(DRM_IOCTL_AMDGPU_INFO) && defined(AMDGPU_INFO_MEMORY)
  struct drm_amdgpu_memory_info drm_info = {};
  struct drm_amdgpu_info buffer = {};

  buffer.query = AMDGPU_INFO_MEMORY;
  buffer.return_pointer = reinterpret_cast<std::uint64_t>(&drm_info);
  buffer.return_size = sizeof(drm_info);

  if (ioctl(deviceFD, DRM_IOCTL_AMDGPU_INFO, &buffer) >= 0) {
    *size = units::make_unit<units::data::megabyte_t>(
        drm_info.vram.total_heap_size / (1024 * 1024));
    return true;
  }
  else
    return false;
#else
  return false;
#endif
}

bool readRadeonVRamSize(int deviceFD, units::data::megabyte_t *size)
{
#if defined(DRM_IOCTL_RADEON_GEM_INFO)
  struct drm_radeon_gem_info buffer = {};
  if (ioctl(deviceFD, DRM_IOCTL_RADEON_GEM_INFO, &buffer) >= 0) {
    *size = units::make_unit<units::data::megabyte_t>(buffer.vram_size /
                                                      (1024 * 1024));
    return true;
  }
  else
    return false;
#else
  return false;
#endif
}

std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseDPMStates(std::vector<std::string> const &ppDpmLines)
{
  // Relevant lines format (kernel 4.6+)
  // 0: 300Mhz *
  // ...
  // N: 1303Mhz
  std::regex const regex(R"(^(\d+)\s*:\s*(\d+)\s*Mhz\s*\*?\s*$)",
                         std::regex::icase);
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;

  for (auto &line : ppDpmLines) {
    std::smatch result;
    if (!std::regex_search(line, result, regex))
      return {};

    unsigned int index{0}, freq{0};
    if (!(Utils::String::toNumber<unsigned int>(index, result[1]) &&
          Utils::String::toNumber<unsigned int>(freq, result[2])))
      return {};

    states.emplace_back(index, units::frequency::megahertz_t(freq));
  }

  if (states.empty())
    return {};

  return std::move(states);
}

std::optional<unsigned int>
parseDPMCurrentStateIndex(std::vector<std::string> const &ppDpmLines)
{
  // Relevant lines format (kernel 4.6+)
  // 0: 300Mhz *
  // ...
  // N: 1303Mhz
  //
  // '*' marks the current state
  std::regex const regex(R"(^(\d+)\s*:\s*\d+\s*Mhz\s*\*\s*$)", std::regex::icase);

  for (auto &line : ppDpmLines) {
    std::smatch result;
    if (std::regex_search(line, result, regex)) {
      unsigned int index{0};
      if (!Utils::String::toNumber<unsigned int>(index, result[1]))
        return {};

      return index;
    }
  }

  return {};
}

bool isPowerProfileModeSupported(
    std::vector<std::string> const &ppPowerProfileModeLines)
{
  // Relevant header formats:
  // NUM ...                          | smu7, vega10
  // PROFILE_INDEX(NAME) ...          | vega20
  if (ppPowerProfileModeLines.empty())
    return false;

  std::regex const regex(R"(^(?:NUM|PROFILE_INDEX\(NAME\))\s+)");
  return std::regex_search(ppPowerProfileModeLines.front(), regex);
}

std::optional<std::vector<std::pair<std::string, int>>>
parsePowerProfileModeModes(std::vector<std::string> const &ppPowerProfileModeLines)
{
  // Relevant lines format
  // ...
  //   1 3D_FULL_SCREEN *:... or  1 3D_FULL_SCREEN*:...
  // ... (other lines on some asics)
  //   2     POWER_SAVING:... or  2   POWER_SAVING :...
  // ...

  if (isPowerProfileModeSupported(ppPowerProfileModeLines)) {

    std::regex const regex(R"(^\s*(\d+)\s*([^\*\s]+)(?:\s|\*)*:)");
    std::vector<std::pair<std::string, int>> modes;

    for (size_t i = 1; i < ppPowerProfileModeLines.size(); ++i) {
      auto &line = ppPowerProfileModeLines[i];

      std::smatch result;
      if (!std::regex_search(line, result, regex))
        continue;

      // skip BOOT and CUSTOM modes
      std::string const mode(result[2]);
      if (mode.find("BOOT") != std::string::npos ||
          mode.find("CUSTOM") != std::string::npos)
        continue;

      int index{0};
      if (!Utils::String::toNumber<int>(index, result[1]))
        continue;

      modes.emplace_back(std::move(mode), index);
    }

    if (!modes.empty())
      return std::move(modes);
  }

  return {};
}

std::optional<int> parsePowerProfileModeCurrentModeIndex(
    std::vector<std::string> const &ppPowerProfileModeLines)
{
  // Relevant lines format
  // ...
  //   1 3D_FULL_SCREEN *:... or  1 3D_FULL_SCREEN*:...
  // ...

  if (isPowerProfileModeSupported(ppPowerProfileModeLines)) {

    std::regex const regex(R"(^\s*(\d+)\s*(?:[^\*\s]+)\s*\*\s*:)");

    // search for selection mark ('*')
    for (size_t i = 1; i < ppPowerProfileModeLines.size(); ++i) {
      auto &line = ppPowerProfileModeLines[i];

      std::smatch result;
      if (!std::regex_search(line, result, regex))
        continue;

      int index{0};
      if (!Utils::String::toNumber<int>(index, result[1]))
        break;

      return index;
    }
  }

  return {};
}

std::optional<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
parseOverdriveClkVoltLine(std::string const &line)
{
  // Relevant lines format (kernel 4.17+):
  // ...
  // 0:    300MHz    800mV
  //
  // On Navi ASICs:
  // ...
  // 0: 300MHz @ 800mV
  // ...
  std::regex const regex(R"((\d+)\s*:\s*(\d+)\s*MHz[\s@]*(\d+)\s*mV\s*$)",
                         std::regex::icase);
  std::smatch result;

  if (std::regex_search(line, result, regex)) {
    unsigned int index{0}, freq{0}, volt{0};
    if (Utils::String::toNumber<unsigned int>(index, result[1]) &&
        Utils::String::toNumber<unsigned int>(freq, result[2]) &&
        Utils::String::toNumber<unsigned int>(volt, result[3]))
      return std::make_tuple(index, units::frequency::megahertz_t(freq),
                             units::voltage::millivolt_t(volt));
  }

  return {};
}

std::optional<std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                     units::voltage::millivolt_t>>>
parseOverdriveClksVolts(std::string_view controlName,
                        std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.17+):
  // ...
  // OD_controlName:
  // ...
  // OD_otherLbl:
  // ...
  auto targetIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_" + std::string(controlName) + ":") !=
               std::string::npos;
      });
  if (targetIt != ppOdClkVoltageLines.cend() &&
      std::next(targetIt) != ppOdClkVoltageLines.cend()) {
    targetIt = std::next(targetIt);

    auto endIt = std::find_if(targetIt, ppOdClkVoltageLines.cend(),
                              [&](std::string const &line) {
                                return line.find("OD_") != std::string::npos;
                              });

    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        states;

    while (targetIt != endIt) {
      auto state = parseOverdriveClkVoltLine(*targetIt);
      if (state.has_value())
        states.emplace_back(std::move(*state));

      targetIt = std::next(targetIt);
    }

    return std::move(states);
  }

  return {};
}

std::optional<std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
parseOverdriveClkRange(std::string const &line)
{
  // Relevant lines format (kernel 4.18+):
  // ...
  // Lbl...: 400MHz 500MHz
  // ...
  std::regex const regex(R"(^(?:[^\:\s]+)\s*:\s*(\d+)\s*MHz\s*(\d+)\s*MHz\s*$)",
                         std::regex::icase);
  std::smatch result;

  if (std::regex_search(line, result, regex)) {
    int min{0}, max{0};
    if (Utils::String::toNumber<int>(min, result[1]) &&
        Utils::String::toNumber<int>(max, result[2]))
      return std::make_pair(units::make_unit<units::frequency::megahertz_t>(min),
                            units::make_unit<units::frequency::megahertz_t>(max));
  }

  return {};
}

std::optional<std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
parseOverdriveClkRange(std::string_view controlName,
                       std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.18+):
  // ...
  // OD_RANGE:
  // ...
  // controlName:     min       max
  // ...
  auto rangeIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_RANGE:") != std::string::npos;
      });
  if (rangeIt != ppOdClkVoltageLines.cend()) {
    auto targetIt = std::find_if(
        rangeIt, ppOdClkVoltageLines.cend(), [&](std::string const &line) {
          return line.find(std::string(controlName) + ":") != std::string::npos;
        });

    if (targetIt != ppOdClkVoltageLines.cend())
      return parseOverdriveClkRange(*targetIt);
  }

  return {};
}

std::optional<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>
parseOverdriveVoltRangeLine(std::string const &line)
{
  // Relevant lines format (kernel 4.18+):
  // ...
  // Lbl...: 400mV 500mV
  // ...
  std::regex const regex(R"(^(?:[^\:\s]+)\s*:\s*(\d+)\s*mV\s*(\d+)\s*mV\s*$)",
                         std::regex::icase);
  std::smatch result;

  if (std::regex_search(line, result, regex)) {
    int min{0}, max{0};
    if (Utils::String::toNumber<int>(min, result[1]) &&
        Utils::String::toNumber<int>(max, result[2]))
      return std::make_pair(units::make_unit<units::voltage::millivolt_t>(min),
                            units::make_unit<units::voltage::millivolt_t>(max));
  }

  return {};
}

std::optional<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>
parseOverdriveVoltRange(std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.18+):
  // ...
  // OD_RANGE:
  // ...
  // VDDC:     min       max
  // ...
  auto rangeIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_RANGE:") != std::string::npos;
      });
  if (rangeIt != ppOdClkVoltageLines.cend()) {
    auto targetIt = std::find_if(
        rangeIt, ppOdClkVoltageLines.cend(), [&](std::string const &line) {
          return line.find("VDDC:") != std::string::npos;
        });

    if (targetIt != ppOdClkVoltageLines.cend())
      return parseOverdriveVoltRangeLine(*targetIt);
  }

  return {};
}

std::optional<std::pair<unsigned int, units::frequency::megahertz_t>>
parseOverdriveClksLine(std::string const &line)
{
  // Relevant lines format (kernel 4.20+):
  // ...
  // 0:    300MHz
  // ...
  std::regex const regex(R"(^(\d+)\s*:\s*(\d+)\s*MHz\s*$)", std::regex::icase);
  std::smatch result;

  if (std::regex_search(line, result, regex)) {
    unsigned int index{0}, freq{0};
    if (Utils::String::toNumber<unsigned int>(index, result[1]) &&
        Utils::String::toNumber<unsigned int>(freq, result[2]))
      return std::make_pair(index, units::frequency::megahertz_t(freq));
  }

  return {};
}

std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseOverdriveClks(std::string_view controlName,
                   std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.17+):
  // ...
  // OD_controlName:
  // ...
  // OD_otherLbl:
  // ...
  auto targetIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_" + std::string(controlName) + ":") !=
               std::string::npos;
      });
  if (targetIt != ppOdClkVoltageLines.cend() &&
      std::next(targetIt) != ppOdClkVoltageLines.cend()) {
    targetIt = std::next(targetIt);

    auto endIt = std::find_if(targetIt, ppOdClkVoltageLines.cend(),
                              [&](std::string const &line) {
                                return line.find("OD_") != std::string::npos;
                              });

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;

    while (targetIt != endIt) {
      auto state = parseOverdriveClksLine(*targetIt);
      if (state.has_value())
        states.emplace_back(std::move(*state));

      targetIt = std::next(targetIt);
    }

    return std::move(states);
  }

  return {};
}

std::optional<std::vector<
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>>
parseOverdriveVoltCurve(std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.20+):
  // ...
  // OD_VDDC_CURVE:
  // 0: 700Mhz 800mV
  // ...
  // OD_RANGE:
  auto targetIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_VDDC_CURVE:") != std::string::npos;
      });
  if (targetIt != ppOdClkVoltageLines.cend() &&
      std::next(targetIt) != ppOdClkVoltageLines.cend()) {
    targetIt = std::next(targetIt);

    auto endIt = std::find_if(targetIt, ppOdClkVoltageLines.cend(),
                              [&](std::string const &line) {
                                return line.find("OD_") != std::string::npos;
                              });

    std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
        points;

    while (targetIt != endIt) {
      auto state = parseOverdriveClkVoltLine(*targetIt);
      if (state.has_value()) {
        auto &[_, freq, volt] = *state;
        points.emplace_back(std::make_pair(freq, volt));
      }

      targetIt = std::next(targetIt);
    }

    return std::move(points);
  }

  return {};
}

std::optional<
    std::vector<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
parseOverdriveVoltCurveRange(std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.20+):
  // ...
  // OD_RANGE:
  // ...
  // VDDC_CURVE_VOLT[0]: 738mV 1218mV
  // ...
  // VDDC_CURVE_VOLT[N]: 738mV 1218mV
  // ...
  auto rangeIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_RANGE:") != std::string::npos;
      });
  if (rangeIt != ppOdClkVoltageLines.cend()) {

    std::vector<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>
        ranges;

    auto targetIt = std::next(rangeIt);
    while (targetIt != ppOdClkVoltageLines.cend()) {
      targetIt = std::find_if(
          targetIt, ppOdClkVoltageLines.cend(), [&](std::string const &line) {
            return line.find("VDDC_CURVE_VOLT[") != std::string::npos;
          });

      if (targetIt != ppOdClkVoltageLines.cend()) {
        auto lineRange = parseOverdriveVoltRangeLine(*targetIt);
        if (lineRange.has_value())
          ranges.emplace_back(std::move(*lineRange));
      }

      targetIt = std::next(targetIt);
    }

    return std::move(ranges);
  }

  return {};
}

std::optional<std::vector<std::string>>
parseOverdriveClkControls(std::vector<std::string> const &ppOdClkVoltageLines)
{
  std::regex const regex(R"(^OD_(\wCLK):\s*$)", std::regex::icase);
  std::vector<std::string> controlNames;

  for (auto &line : ppOdClkVoltageLines) {
    std::smatch result;
    if (!std::regex_search(line, result, regex))
      continue;

    controlNames.emplace_back(result[1]);
  }

  if (!controlNames.empty())
    return controlNames;

  return {};
}

std::optional<std::string>
getOverdriveClkControlCmdId(std::string_view controlName)
{
  static std::unordered_map<std::string_view, std::string> const nameCmdIdMap{
      {"SCLK", "s"}, {"MCLK", "m"}};

  if (nameCmdIdMap.count(controlName) > 0)
    return nameCmdIdMap.at(controlName);

  return {};
}

bool ppOdClkVoltageHasKnownQuirks(
    std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Empty file
  if (ppOdClkVoltageLines.empty())
    return true;

  // Check for missing range section (kernel < 4.18)
  auto odRangeIter = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) { return line == "OD_RANGE:"; });
  if (odRangeIter == ppOdClkVoltageLines.cend())
    return true;

  // Check for voltage incomplete curve points (navi on kernel < 5.6)
  // "OD_VDDC_CURVE:",
  // "0: 700Mhz @ 0mV",
  auto atIter = std::find_if(ppOdClkVoltageLines.cbegin(),
                             ppOdClkVoltageLines.cend(),
                             [&](std::string const &line) {
                               return line.find("@") != std::string::npos;
                             });
  if (atIter != ppOdClkVoltageLines.cend()) {
    auto points = parseOverdriveVoltCurve(ppOdClkVoltageLines);
    if (!points.has_value())
      return true;

    return points->at(0).second == units::voltage::millivolt_t(0);
  }

  return false;
}

bool hasOverdriveClkVoltControl(std::vector<std::string> const &data)
{
  std::regex const clkRegex(R"(^OD_\wCLK:)", std::regex::icase);
  std::smatch result;

  auto clkIt = std::find_if(data.cbegin(), data.cend(),
                            [&](std::string const &line) {
                              return std::regex_match(line, result, clkRegex);
                            });

  if (clkIt != data.cend() && std::next(clkIt) != data.cend()) {
    auto state = parseOverdriveClkVoltLine(*std::next(clkIt));
    return state.has_value();
  }

  return false;
}

bool hasOverdriveClkControl(std::vector<std::string> const &data)
{
  std::regex const clkRegex(R"(^OD_\wCLK:)", std::regex::icase);
  std::smatch result;

  auto clkIt = std::find_if(data.cbegin(), data.cend(),
                            [&](std::string const &line) {
                              return std::regex_match(line, result, clkRegex);
                            });

  if (clkIt != data.cend() && std::next(clkIt) != data.cend()) {
    auto state = parseOverdriveClksLine(*std::next(clkIt));
    return state.has_value();
  }

  return false;
}

bool hasOverdriveVoltCurveControl(std::vector<std::string> const &data)
{
  auto curveIt = std::find_if(
      data.cbegin(), data.cend(), [&](std::string const &line) {
        return line.find("OD_VDDC_CURVE:") != std::string::npos;
      });

  return curveIt != data.cend();
}

} // namespace AMD
} // namespace Utils
