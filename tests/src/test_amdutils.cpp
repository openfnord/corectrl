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
#include "catch.hpp"

#include "core/components/amdutils.h"

namespace Tests {
namespace Utils {
namespace AMD {

TEST_CASE("AMD utils tests", "[Utils][AMD]")
{
  SECTION("parseDPMStates")
  {
    // clang-format off
    std::vector<std::string> input{"0: 300Mhz *",
                                   "1: 2000Mhz"};
    // clang-format on

    SECTION("Returns dpm states indices and frequencies")
    {
      auto states = ::Utils::AMD::parseDPMStates(input);
      REQUIRE(states.has_value());
      REQUIRE(states.value().size() == 2);

      auto &[s0Index, s0Freq] = states.value().front();
      REQUIRE(s0Index == 0);
      REQUIRE(s0Freq == units::frequency::megahertz_t(300));

      auto &[s1Index, s1Freq] = states.value().back();
      REQUIRE(s1Index == 1);
      REQUIRE(s1Freq == units::frequency::megahertz_t(2000));
    }

    SECTION("Returns no dpm states for invalid input")
    {
      // clang-format off
      std::vector<std::string> input{""};
      // clang-format on

      auto states = ::Utils::AMD::parseDPMStates(input);
      REQUIRE_FALSE(states.has_value());
    }
  }

  SECTION("parseDPMCurrentStateIndex")
  {
    // clang-format off
    std::vector<std::string> input{"0: 300Mhz",
                                   "1: 2000Mhz *"};
    // clang-format on

    SECTION("Returns index of current dpm state")
    {
      auto index = ::Utils::AMD::parseDPMCurrentStateIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns nothing for invalid input")
    {
      // clang-format off
      std::vector<std::string> input{""};
      // clang-format on

      auto index = ::Utils::AMD::parseDPMCurrentStateIndex(input);
      REQUIRE_FALSE(index.has_value());
    }
  }

  SECTION("parsePowerProfileModeModes")
  {
    SECTION("Returns power profile modes on smu7 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "NUM        MODE_NAME     SCLK_UP_HYST   SCLK_DOWN_HYST SCLK_ACTIVE_LEVEL     MCLK_UP_HYST   MCLK_DOWN_HYST MCLK_ACTIVE_LEVEL",
          "  0   BOOTUP_DEFAULT:        -                -                -                -                -                -",
          "  1 3D_FULL_SCREEN *:        0              100               30                0              100               10",
          "  2     POWER_SAVING:       10                0               30                -                -                -",
          "...",
          "  6           CUSTOM:        -                -                -                -                -                -"};
      // clang-format on

      auto modes = ::Utils::AMD::parsePowerProfileModeModes(input);
      REQUIRE(modes.has_value());
      REQUIRE_FALSE(modes->empty());

      auto &[mode0, mode0Index] = modes->at(0);
      REQUIRE(mode0 == "3D_FULL_SCREEN");
      REQUIRE(mode0Index == 1);

      auto &[mode1, mode1Index] = modes->at(1);
      REQUIRE(mode1 == "POWER_SAVING");
      REQUIRE(mode1Index == 2);
    }

    SECTION("Returns power profile modes on vega10 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "NUM        MODE_NAME BUSY_SET_POINT FPS USE_RLC_BUSY MIN_ACTIVE_LEVEL",
          "  0 3D_FULL_SCREEN*:             70  60          1              3",
          "  1   POWER_SAVING :             90  60          0              0",
          "...",
          "  5         CUSTOM :              0   0          0              0"};
      // clang-format on

      auto modes = ::Utils::AMD::parsePowerProfileModeModes(input);
      REQUIRE(modes.has_value());
      REQUIRE_FALSE(modes->empty());

      auto &[mode0, mode0Index] = modes->at(0);
      REQUIRE(mode0 == "3D_FULL_SCREEN");
      REQUIRE(mode0Index == 0);

      auto &[mode1, mode1Index] = modes->at(1);
      REQUIRE(mode1 == "POWER_SAVING");
      REQUIRE(mode1Index == 1);
    }

    SECTION("Returns power profile modes on vega20 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "PROFILE_INDEX(NAME)  ...",
          "  0 3D_FULL_SCREEN*:",
          "...",
          "  1   POWER_SAVING :",
          "...",
          "  5         CUSTOM :",
          "..."};
      // clang-format on

      auto modes = ::Utils::AMD::parsePowerProfileModeModes(input);
      REQUIRE(modes.has_value());
      REQUIRE_FALSE(modes->empty());

      auto &[mode0, mode0Index] = modes->at(0);
      REQUIRE(mode0 == "3D_FULL_SCREEN");
      REQUIRE(mode0Index == 0);

      auto &[mode1, mode1Index] = modes->at(1);
      REQUIRE(mode1 == "POWER_SAVING");
      REQUIRE(mode1Index == 1);
    }
  }

  SECTION("parsePowerProfileModeCurrentModeIndex")
  {
    SECTION("Returns index of current profile mode on smu7 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "NUM        MODE_NAME     SCLK_UP_HYST   SCLK_DOWN_HYST SCLK_ACTIVE_LEVEL     MCLK_UP_HYST   MCLK_DOWN_HYST MCLK_ACTIVE_LEVEL",
          "  0   BOOTUP_DEFAULT:        -                -                -                -                -                -",
          "  1 3D_FULL_SCREEN *:        0              100               30                0              100               10",
          "  2     POWER_SAVING:       10                0               30                -                -                -",
          "...",
          "  6           CUSTOM:        -                -                -                -                -                -"};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns index of current profile mode on vega10 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "NUM        MODE_NAME BUSY_SET_POINT FPS USE_RLC_BUSY MIN_ACTIVE_LEVEL",
          "  0 3D_FULL_SCREEN :             70  60          1              3",
          "  1   POWER_SAVING*:             90  60          0              0",
          "...",
          "  5         CUSTOM :              0   0          0              0"};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns index of current profile mode on vega20 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "PROFILE_INDEX(NAME)  ...",
          "  0 3D_FULL_SCREEN :",
          "...",
          "  1   POWER_SAVING*:",
          "...",
          "  5         CUSTOM :",
          "..."};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns nothing for invalid input")
    {
      // clang-format off
      std::vector<std::string> input{""};
      // clang-format on

      auto index = ::Utils::AMD::parseDPMCurrentStateIndex(input);
      REQUIRE_FALSE(index.has_value());
    }
  }

  SECTION("parseOdClkVoltStateStates")
  {
    // clang-format off
    std::vector<std::string> input{"OD_MCLK:",
                                   "0:        300MHz        800mV",
                                   "1:       2000MHz        975mV",
                                   "OD_RANGE:"};
    // clang-format on

    SECTION("Returns target states")
    {
      auto values = ::Utils::AMD::parseOdClkVoltStateStates("MCLK", input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      auto &[s0Idx, s0Freq, s0Volt] = values->at(0);
      REQUIRE(s0Idx == 0);
      REQUIRE(s0Freq == units::frequency::megahertz_t(300));
      REQUIRE(s0Volt == units::voltage::millivolt_t(800));

      auto &[s1Idx, s1Freq, s1Volt] = values->at(1);
      REQUIRE(s1Idx == 1);
      REQUIRE(s1Freq == units::frequency::megahertz_t(2000));
      REQUIRE(s1Volt == units::voltage::millivolt_t(975));
    }

    SECTION("Returns nothing when there is no OD_target in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "0:        300MHz        800mV",
                                     "1:       2000MHz        975mV",
                                     "OD_RANGE:"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOdClkVoltStateStates("MCLK", input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOdClkVoltStateClkRange")
  {
    // clang-format off
    std::vector<std::string> input{"OD_RANGE:",
                                   "SCLK:     300MHz       2000MHz",
                                   "MCLK:     300MHz       2250MHz"};
    // clang-format on

    SECTION("Returns minimum and maximum gpu frequency")
    {
      auto values = ::Utils::AMD::parseOdClkVoltStateClkRange("SCLK", input);
      REQUIRE(values.has_value());
      REQUIRE(values->first == units::frequency::megahertz_t(300));
      REQUIRE(values->second == units::frequency::megahertz_t(2000));
    }

    SECTION("Returns minimum and maximum memory frequency")
    {
      auto values = ::Utils::AMD::parseOdClkVoltStateClkRange("MCLK", input);
      REQUIRE(values.has_value());
      REQUIRE(values->first == units::frequency::megahertz_t(300));
      REQUIRE(values->second == units::frequency::megahertz_t(2250));
    }

    SECTION("Returns nothing when there is no OD_RANGE in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "SCLK:     300MHz       2000MHz",
                                     "MCLK:     300MHz       2250MHz"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOdClkVoltStateClkRange("SLCK", input);
      REQUIRE_FALSE(empty.has_value());
    }

    SECTION("Returns nothing for wrong target label")
    {
      auto empty = ::Utils::AMD::parseOdClkVoltStateClkRange("OTHER", input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOdClkVoltStateVoltRange")
  {
    // clang-format off
    std::vector<std::string> input{"OD_RANGE:",
                                   "VDDC:     800mV        1175mV"};
    // clang-format on

    SECTION("Returns minimum and maximum state voltage")
    {
      auto values = ::Utils::AMD::parseOdClkVoltStateVoltRange(input);
      REQUIRE(values.has_value());
      REQUIRE(values->first == units::voltage::millivolt_t(800));
      REQUIRE(values->second == units::voltage::millivolt_t(1175));
    }

    SECTION("Returns nothing when there is no OD_RANGE in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "VDDC:     800mV        1175mV"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOdClkVoltStateVoltRange(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOdClkVoltCurveStates")
  {
    // clang-format off
    std::vector<std::string> input{"OD_SCLK:",
                                   "0:        300MHz",
                                   "1:       2000MHz",
                                   "OD_MCLK:"};
    // clang-format on

    SECTION("Returns target states")
    {
      auto values = ::Utils::AMD::parseOdClkVoltCurveStates("SCLK", input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      auto &[s0Idx, s0Freq] = values->at(0);
      REQUIRE(s0Idx == 0);
      REQUIRE(s0Freq == units::frequency::megahertz_t(300));

      auto &[s1Idx, s1Freq] = values->at(1);
      REQUIRE(s1Idx == 1);
      REQUIRE(s1Freq == units::frequency::megahertz_t(2000));
    }

    SECTION("Returns nothing when there is no OD_target in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "0:        300MHz",
                                     "1:       2000MHz",
                                     "OD_MCLK:"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOdClkVoltCurveStates("SCLK", input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOdClkVoltCurvePoints")
  {
    // clang-format off
    std::vector<std::string> input{"OD_VDDC_CURVE:",
                                   "0: 700Mhz 800mV",
                                   "2: 800Mhz @ 900mV", // navi
                                   "OD_RANGE:"};
    // clang-format on

    SECTION("Returns target states")
    {
      auto values = ::Utils::AMD::parseOdClkVoltCurvePoints(input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      auto &[p0Freq, p0Volt] = values->at(0);
      REQUIRE(p0Freq == units::frequency::megahertz_t(700));
      REQUIRE(p0Volt == units::voltage::millivolt_t(800));

      auto &[p1Freq, p1Volt] = values->at(1);
      REQUIRE(p1Freq == units::frequency::megahertz_t(800));
      REQUIRE(p1Volt == units::voltage::millivolt_t(900));
    }

    SECTION("Returns nothing when there is no OD_VDDC_CURVE: in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "0: 700Mhz 800mV",
                                     "2: 800Mhz 900mV",
                                     "OD_RANGE:"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOdClkVoltCurvePoints(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOdClkVoltCurveVoltRange")
  {
    // clang-format off
    std::vector<std::string> input{"OD_RANGE:",
                                   "VDDC_CURVE_SCLK[0]: 800Mhz 2000Mhz",
                                   "VDDC_CURVE_VOLT[0]: 700mV 1200mV",
                                   "VDDC_CURVE_SCLK[1]: 800Mhz 2000Mhz",
                                   "VDDC_CURVE_VOLT[1]: 800mV 1300mV"};
    // clang-format on

    SECTION("Returns curve points voltage")
    {
      auto values = ::Utils::AMD::parseOdClkVoltCurveVoltRange(input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      auto &[p0min, p0max] = values->at(0);
      REQUIRE(p0min == units::voltage::millivolt_t(700));
      REQUIRE(p0max == units::voltage::millivolt_t(1200));

      auto &[p1min, p1max] = values->at(1);
      REQUIRE(p1min == units::voltage::millivolt_t(800));
      REQUIRE(p1max == units::voltage::millivolt_t(1300));
    }

    SECTION("Returns nothing when there is no OD_RANGE in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "VDDC_CURVE_SCLK[0]: 800Mhz 2000Mhz",
                                     "VDDC_CURVE_VOLT[0]: 700mV 1200mV",
                                     "VDDC_CURVE_SCLK[1]: 800Mhz 2000Mhz",
                                     "VDDC_CURVE_VOLT[1]: 800mV 1300mV"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOdClkVoltStateVoltRange(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("ppOdClkVoltageHasKnownQuirks")
  {
    SECTION("Empty file")
    {
      REQUIRE(::Utils::AMD::ppOdClkVoltageHasKnownQuirks(
          std::vector<std::string>()));
    }

    SECTION("Pre-Vega20 missing range section")
    {
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
                                     "0:        300MHz        800mV",
                                     "1:        608MHz        818mV",
                                     "OD_MCLK:",
                                     "0:        300MHz        800mV",
                                     "1:       2000MHz        975mV"};
      // clang-format on
      REQUIRE(::Utils::AMD::ppOdClkVoltageHasKnownQuirks(input));
    }

    SECTION("Navi zero voltage curve points")
    {
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
                                     "0: 800Mhz",
                                     "1: 2100Mhz",
                                     "OD_MCLK:",
                                     "1: 875MHz",
                                     "OD_VDDC_CURVE:",
                                     "0: 700Mhz @ 0mV",
                                     "1: 800Mhz @ 0mV",
                                     "OD_RANGE:",
                                     "SCLK:     800Mhz       2150Mhz",
                                     "MCLK:     625Mhz        950Mhz",
                                     "VDDC_CURVE_SCLK[0]:     800Mhz       2150Mhz",
                                     "VDDC_CURVE_VOLT[0]:     750mV        1200mV",
                                     "VDDC_CURVE_SCLK[1]:     800Mhz       2150Mhz",
                                     "VDDC_CURVE_VOLT[1]:     750mV        1200mV",
                                     "VDDC_CURVE_SCLK[2]:     800Mhz       2150Mhz",
                                     "VDDC_CURVE_VOLT[2]:     750mV        1200mV"};
      // clang-format on
      REQUIRE(::Utils::AMD::ppOdClkVoltageHasKnownQuirks(input));
    }

    SECTION("Good input has no quirks")
    {
      // clang-format off
      std::vector<std::string> preVega20Input{"OD_SCLK:",
                                              "0:        300MHz        800mV",
                                              "1:        608MHz        818mV",
                                              "OD_MCLK:",
                                              "0:        300MHz        800mV",
                                              "1:       2000MHz        975mV",
                                              "OD_RANGE:",
                                              "SCLK:     800MHz       2150MHz",
                                              "MCLK:     625MHz        950MHz",
                                              "VDDC:     800mV        1175mV"};
      // clang-format on
      REQUIRE_FALSE(::Utils::AMD::ppOdClkVoltageHasKnownQuirks(preVega20Input));

      // Vega20
      // clang-format off
      std::vector<std::string> vega20Input{"OD_SCLK:",
                                           "0: 800Mhz",
                                           "1: 2100Mhz",
                                           "OD_MCLK:",
                                           "1: 875MHz",
                                           "OD_VDDC_CURVE:",
                                           "0: 700Mhz 800mV",
                                           "1: 800Mhz 900mV",
                                           "OD_RANGE:",
                                           "SCLK:     800Mhz       2150Mhz",
                                           "MCLK:     625Mhz        950Mhz",
                                           "VDDC_CURVE_SCLK[0]:     800Mhz       2150Mhz",
                                           "VDDC_CURVE_VOLT[0]:     750mV        1200mV",
                                           "VDDC_CURVE_SCLK[1]:     800Mhz       2150Mhz",
                                           "VDDC_CURVE_VOLT[1]:     750mV        1200mV",
                                           "VDDC_CURVE_SCLK[2]:     800Mhz       2150Mhz",
                                           "VDDC_CURVE_VOLT[2]:     750mV        1200mV"};
      // clang-format on
      REQUIRE_FALSE(::Utils::AMD::ppOdClkVoltageHasKnownQuirks(vega20Input));

      // Navi
      // clang-format off
      std::vector<std::string> naviInput{"OD_SCLK:",
                                         "0: 800Mhz",
                                         "1: 2100Mhz",
                                         "OD_MCLK:",
                                         "1: 875MHz",
                                         "OD_VDDC_CURVE:",
                                         "0: 700Mhz @ 800mV",
                                         "1: 800Mhz @ 900mV",
                                         "OD_RANGE:",
                                         "SCLK:     800Mhz       2150Mhz",
                                         "MCLK:     625Mhz        950Mhz",
                                         "VDDC_CURVE_SCLK[0]:     800Mhz       2150Mhz",
                                         "VDDC_CURVE_VOLT[0]:     750mV        1200mV",
                                         "VDDC_CURVE_SCLK[1]:     800Mhz       2150Mhz",
                                         "VDDC_CURVE_VOLT[1]:     750mV        1200mV",
                                         "VDDC_CURVE_SCLK[2]:     800Mhz       2150Mhz",
                                         "VDDC_CURVE_VOLT[2]:     750mV        1200mV"};
      // clang-format on
      REQUIRE_FALSE(::Utils::AMD::ppOdClkVoltageHasKnownQuirks(naviInput));
    }
  }
}
} // namespace AMD
} // namespace Utils
} // namespace Tests
