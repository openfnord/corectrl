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
#include <algorithm>
#include <cstddef>
#include <iterator>

namespace Utils {
namespace AMD {

std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseDPMStates(std::vector<std::string> const &ppDpmLines)
{
  // Relevant lines format (kernel 4.6+)
  // 0: 300Mhz *
  // ...
  // N: 1303Mhz
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;

  for (auto &line : ppDpmLines) {
    auto colonPos = line.find(':');
    if (colonPos == std::string::npos)
      return {};

    unsigned int index = 0;
    auto indexStr = line.substr(0, colonPos);
    if (!Utils::String::toNumber<unsigned int>(index, indexStr))
      return {};

    auto spacePos = line.find(' ', colonPos);
    if (spacePos == std::string::npos)
      return {};

    auto mhzPos = line.find("M", spacePos);
    if (mhzPos == std::string::npos)
      return {};

    int freq = 0;
    auto freqStr = line.substr(spacePos, mhzPos - spacePos);
    if (!Utils::String::toNumber<int>(freq, freqStr))
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

  for (auto &line : ppDpmLines) {
    auto markerPos = line.find('*');
    if (markerPos != std::string::npos) {

      auto colonPos = line.find(':');
      if (colonPos == std::string::npos)
        return {};

      unsigned int index = 0;
      auto indexStr = line.substr(0, colonPos);
      if (!Utils::String::toNumber<unsigned int>(index, indexStr))
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
  // NUM ...
  // PROFILE_INDEX(NAME) ...

  if (ppPowerProfileModeLines.empty())
    return false;

  // check the first word of the header for known words
  auto spacePos = ppPowerProfileModeLines.front().find(' ');
  if (spacePos == std::string::npos)
    return false;

  auto firstWord = ppPowerProfileModeLines.front().substr(0, spacePos);
  if (!(firstWord == "NUM" ||                // smu7, vega10
        firstWord == "PROFILE_INDEX(NAME)")) // vega20
    return false;

  return true;
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

    std::vector<std::pair<std::string, int>> modes;
    for (size_t i = 1; i < ppPowerProfileModeLines.size(); ++i) {
      auto &line = ppPowerProfileModeLines[i];

      // assuming that no other lines but the lines that contains the mode name has ':'
      auto colonPos = line.find(':');
      if (colonPos == std::string::npos)
        continue;

      auto modeStr = line.substr(0, colonPos);
      auto indexPos = modeStr.find_first_not_of(' ');
      auto spacePos = modeStr.find(' ', indexPos);

      int index{0};
      if (!Utils::String::toNumber<int>(
              index, modeStr.substr(indexPos, spacePos - indexPos)))
        continue;

      auto modePos = modeStr.find_first_not_of(' ', spacePos);
      auto modeEndPos = modeStr.size();
      if (modeStr.back() == '*')
        --modeEndPos;

      auto mode = modeStr.substr(modePos, modeEndPos - modePos);

      // remove remaining spaces
      mode.erase(std::remove(mode.begin(), mode.end(), ' '), mode.end());

      // skip BOOT and CUSTOM modes
      if (mode.find("BOOT") != std::string::npos ||
          mode.find("CUSTOM") != std::string::npos)
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

    // search for selection mark ('*')
    for (size_t i = 1; i < ppPowerProfileModeLines.size(); ++i) {
      auto &line = ppPowerProfileModeLines[i];

      auto markPos = line.find('*');
      if (markPos != std::string::npos) {
        auto indexPos = line.find_first_not_of(' ');
        auto spacePos = line.find(' ', indexPos);

        int index{0};
        if (!Utils::String::toNumber<int>(
                index, line.substr(indexPos, spacePos - indexPos)))
          break;

        return index;
      }
    }
  }

  return {};
}

std::optional<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
parseOdClkVoltStateStatesLine(std::string const &line)
{
  // Relevant lines format (kernel 4.17+):
  // ...
  // 0:    300MHz    800mV
  // ...

  // index
  auto colonPos = line.find(':');
  auto indexStr = line.substr(0, colonPos);

  // frequency
  auto spacePos = line.find(' ', colonPos);
  auto valuePos = line.find_first_not_of(' ', spacePos);
  auto endValuePos = line.find(' ', valuePos);
  auto freqStr = line.substr(valuePos, endValuePos - valuePos);

  // voltage
  valuePos = line.find_first_not_of(' ', endValuePos);
  auto voltStr = line.substr(valuePos);

  unsigned int index{0}, freq{0}, volt{0};
  if (Utils::String::toNumber<unsigned int>(index, indexStr) &&
      Utils::String::toNumber<unsigned int>(freq, freqStr) &&
      Utils::String::toNumber<unsigned int>(volt, voltStr))
    return std::make_tuple(index, units::frequency::megahertz_t(freq),
                           units::voltage::millivolt_t(volt));

  return {};
}

std::optional<std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                     units::voltage::millivolt_t>>>
parseOdClkVoltStateStates(std::string_view targetLbl,
                          std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.17+):
  // ...
  // OD_targetLbl:
  // ...
  // OD_otherLbl:
  // ...
  auto targetIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_" + std::string(targetLbl) + ":") !=
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
      auto state = parseOdClkVoltStateStatesLine(*targetIt);
      if (state.has_value())
        states.emplace_back(std::move(*state));

      targetIt = std::next(targetIt);
    }

    return std::move(states);
  }

  return {};
}

template<typename T>
std::optional<std::pair<T, T>> parseOdClkVoltRangeLine(std::string const &line)
{
  // Relevant lines format (kernel 4.18+):
  // ...
  // targetLbl...: min max
  // ...

  // min value
  auto spacePos = line.find(' ');
  auto valuePos = line.find_first_not_of(' ', spacePos);
  auto endValuePos = line.find(' ', valuePos);
  auto minStr = line.substr(valuePos, endValuePos - valuePos);

  // max value
  valuePos = line.find_first_not_of(' ', endValuePos);
  auto maxStr = line.substr(valuePos);

  int min{0}, max{0};
  if (Utils::String::toNumber<int>(min, minStr) &&
      Utils::String::toNumber<int>(max, maxStr))
    return std::make_pair(units::make_unit<T>(min), units::make_unit<T>(max));

  return {};
}

template<typename T>
std::optional<std::pair<T, T>>
parseOdClkVoltStateRange(std::string_view targetLbl,
                         std::vector<std::string> const &dataSourceLines)
{
  // Relevant lines format (kernel 4.18+):
  // ...
  // OD_RANGE:
  // ...
  // targetLbl:     min       max
  // ...
  auto rangeIt = std::find_if(dataSourceLines.cbegin(), dataSourceLines.cend(),
                              [&](std::string const &line) {
                                return line.find("OD_RANGE:") !=
                                       std::string::npos;
                              });
  if (rangeIt != dataSourceLines.cend()) {
    auto targetIt = std::find_if(
        rangeIt, dataSourceLines.cend(), [&](std::string const &line) {
          return line.find(std::string(targetLbl) + ":") != std::string::npos;
        });

    if (targetIt != dataSourceLines.cend())
      return parseOdClkVoltRangeLine<T>(*targetIt);
  }

  return {};
}

std::optional<std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
parseOdClkVoltStateClkRange(std::string_view targetLbl,
                            std::vector<std::string> const &ppOdClkVoltageLines)
{
  return parseOdClkVoltStateRange<units::frequency::megahertz_t>(
      targetLbl, ppOdClkVoltageLines);
}

std::optional<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>
parseOdClkVoltStateVoltRange(std::vector<std::string> const &ppOdClkVoltageLines)
{
  return parseOdClkVoltStateRange<units::voltage::millivolt_t>(
      "VDDC", ppOdClkVoltageLines);
}

std::optional<std::pair<unsigned int, units::frequency::megahertz_t>>
parseOdClkVoltCurveStatesLine(std::string const &line)
{
  // Relevant lines format (kernel 4.20+):
  // ...
  // 0:    300MHz
  // ...

  // index
  auto colonPos = line.find(':');
  auto indexStr = line.substr(0, colonPos);

  // frequency
  auto spacePos = line.find(' ', colonPos);
  auto valuePos = line.find_first_not_of(' ', spacePos);
  auto freqStr = line.substr(valuePos);

  unsigned int index{0}, freq{0};
  if (Utils::String::toNumber<unsigned int>(index, indexStr) &&
      Utils::String::toNumber<unsigned int>(freq, freqStr))
    return std::make_pair(index, units::frequency::megahertz_t(freq));

  return {};
}

std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseOdClkVoltCurveStates(std::string_view targetLbl,
                          std::vector<std::string> const &ppOdClkVoltageLines)
{
  // Relevant lines format (kernel 4.17+):
  // ...
  // OD_targetLbl:
  // ...
  // OD_otherLbl:
  // ...
  auto targetIt = std::find_if(
      ppOdClkVoltageLines.cbegin(), ppOdClkVoltageLines.cend(),
      [&](std::string const &line) {
        return line.find("OD_" + std::string(targetLbl) + ":") !=
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
      auto state = parseOdClkVoltCurveStatesLine(*targetIt);
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
parseOdClkVoltCurvePoints(std::vector<std::string> const &ppOdClkVoltageLines)
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
      auto state = parseOdClkVoltStateStatesLine(*targetIt);
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
parseOdClkVoltCurveVoltRange(std::vector<std::string> const &ppOdClkVoltageLines)
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
        auto lineRange =
            parseOdClkVoltRangeLine<units::voltage::millivolt_t>(*targetIt);
        if (lineRange.has_value())
          ranges.emplace_back(std::move(*lineRange));
      }

      targetIt = std::next(targetIt);
    }

    return std::move(ranges);
  }

  return {};
}

} // namespace AMD
} // namespace Utils
