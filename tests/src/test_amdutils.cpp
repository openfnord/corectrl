// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/amdutils.h"
#include "units/units.h"

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
          "NUM ...",
          "  0   BOOTUP_DEFAULT: ...",
          "  1 3D_FULL_SCREEN *: ...",
          "  2     POWER_SAVING: ...",
          "...",
          "  6           CUSTOM: ..."};
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
          "NUM ...",
          "  0 3D_FULL_SCREEN*: ...",
          "  1   POWER_SAVING : ...",
          "...",
          "  5         CUSTOM : ..."};
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
          "  0 3D_FULL_SCREEN*: ...",
          "  1   POWER_SAVING : ...",
          "...",
          "  5         CUSTOM : ..."};
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

    SECTION("Returns power profile modes on sienna cichlid asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "PROFILE_INDEX(NAME)  ...",
          "...",
          "  1 3D_FULL_SCREEN*: ...",
          "                    0(       GFXCLK) ...",
          "...",
          "  2   POWER_SAVING : ...",
          "                    0(       GFXCLK) ...",
          "..."};
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

    SECTION("Returns power profile modes on asics without heuristics settings")
    {
      // clang-format off
      std::vector<std::string> input{
          "  0 3D_FULL_SCREEN ",
          "  3          VIDEO*",
          "...",
          "  6         CUSTOM "};
      // clang-format on

      auto modes = ::Utils::AMD::parsePowerProfileModeModes(input);
      REQUIRE(modes.has_value());
      REQUIRE_FALSE(modes->empty());

      auto &[mode0, mode0Index] = modes->at(0);
      REQUIRE(mode0 == "3D_FULL_SCREEN");
      REQUIRE(mode0Index == 0);

      auto &[mode1, mode1Index] = modes->at(1);
      REQUIRE(mode1 == "VIDEO");
      REQUIRE(mode1Index == 3);
    }
  }

  SECTION("parsePowerProfileModeCurrentModeIndex")
  {
    SECTION("Returns index of current profile mode on smu7 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "NUM ...",
          "  0   BOOTUP_DEFAULT: ...",
          "  1 3D_FULL_SCREEN *: ...",
          "  2     POWER_SAVING: ...",
          "...",
          "  6           CUSTOM: ..."};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns index of current profile mode on vega10 asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "NUM ...",
          "  0 3D_FULL_SCREEN : ...",
          "  1   POWER_SAVING*: ...",
          "...",
          "  5         CUSTOM : ..."};
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
          "  0 3D_FULL_SCREEN : ...",
          "  1   POWER_SAVING*: ...",
          "...",
          "  5         CUSTOM : ..."};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns index of current profile mode on sienna cichlid asics")
    {
      // clang-format off
      std::vector<std::string> input{
          "PROFILE_INDEX(NAME)  ...",
          "...",
          "  1 3D_FULL_SCREEN*: ...",
          "                    0(       GFXCLK) ...",
          "...",
          "  2   POWER_SAVING : ...",
          "                    0(       GFXCLK) ...",
          "..."};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 1);
    }

    SECTION("Returns index of current profile mode on asics without heuristics "
            "settings")
    {
      // clang-format off
      std::vector<std::string> input{
          "  0 3D_FULL_SCREEN ",
          "  3          VIDEO*",
          "...",
          "  6         CUSTOM "};
      // clang-format on

      auto index = ::Utils::AMD::parsePowerProfileModeCurrentModeIndex(input);
      REQUIRE(index.has_value());
      REQUIRE(*index == 3);
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

  SECTION("parseOverdriveClksVolts")
  {
    // clang-format off
    std::vector<std::string> input{"OD_MCLK:",
                                   "0:        300MHz        800mV",
                                   "1:       2000MHz        975mV",
                                   "OD_RANGE:"};
    // clang-format on

    SECTION("Returns the available states for the control")
    {
      auto values = ::Utils::AMD::parseOverdriveClksVolts("MCLK", input);
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

    SECTION("Returns nothing when there is no OD_controlName in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "0:        300MHz        800mV",
                                     "1:       2000MHz        975mV",
                                     "OD_RANGE:"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOverdriveClksVolts("MCLK", input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOverdriveClkRange")
  {
    // clang-format off
    std::vector<std::string> input{"OD_RANGE:",
                                   "SCLK:     300MHz       2000MHz",
                                   "MCLK:     300MHz       2250MHz"};
    // clang-format on

    SECTION("Returns minimum and maximum gpu frequency")
    {
      auto values = ::Utils::AMD::parseOverdriveClkRange("SCLK", input);
      REQUIRE(values.has_value());
      REQUIRE(values->first == units::frequency::megahertz_t(300));
      REQUIRE(values->second == units::frequency::megahertz_t(2000));
    }

    SECTION("Returns minimum and maximum memory frequency")
    {
      auto values = ::Utils::AMD::parseOverdriveClkRange("MCLK", input);
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

      auto empty = ::Utils::AMD::parseOverdriveClkRange("SLCK", input);
      REQUIRE_FALSE(empty.has_value());
    }

    SECTION("Returns nothing for unknown controls names")
    {
      auto empty = ::Utils::AMD::parseOverdriveClkRange("OTHER", input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOverdriveVoltRange")
  {
    // clang-format off
    std::vector<std::string> input{"OD_RANGE:",
                                   "VDDC:     800mV        1175mV"};
    // clang-format on

    SECTION("Returns minimum and maximum state voltage")
    {
      auto values = ::Utils::AMD::parseOverdriveVoltRange(input);
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

      auto empty = ::Utils::AMD::parseOverdriveVoltRange(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOverdriveClks")
  {
    // clang-format off
    std::vector<std::string> input{"OD_SCLK:",
                                   "0:        300MHz",
                                   "1:       2000MHz",
                                   "OD_MCLK:"};
    // clang-format on

    SECTION("Returns available states for the control")
    {
      auto values = ::Utils::AMD::parseOverdriveClks("SCLK", input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      auto &[s0Idx, s0Freq] = values->at(0);
      REQUIRE(s0Idx == 0);
      REQUIRE(s0Freq == units::frequency::megahertz_t(300));

      auto &[s1Idx, s1Freq] = values->at(1);
      REQUIRE(s1Idx == 1);
      REQUIRE(s1Freq == units::frequency::megahertz_t(2000));
    }

    SECTION("Returns nothing when there is no OD_controlName in input")
    {
      // clang-format off
      std::vector<std::string> input{"OTHER:",
                                     "0:        300MHz",
                                     "1:       2000MHz",
                                     "OD_MCLK:"};
      // clang-format on

      auto empty = ::Utils::AMD::parseOverdriveClks("SCLK", input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOverdriveVoltCurve")
  {
    // clang-format off
    std::vector<std::string> input{"OD_VDDC_CURVE:",
                                   "0: 700Mhz 800mV",
                                   "2: 800Mhz @ 900mV", // navi
                                   "OD_RANGE:"};
    // clang-format on

    SECTION("Returns the available voltage curve points")
    {
      auto values = ::Utils::AMD::parseOverdriveVoltCurve(input);
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

      auto empty = ::Utils::AMD::parseOverdriveVoltCurve(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOverdriveVoltCurveRange")
  {
    // clang-format off
    std::vector<std::string> input{"OD_RANGE:",
                                   "...",
                                   "VDDC_CURVE_SCLK[0]: 800Mhz 2000Mhz",
                                   "VDDC_CURVE_VOLT[0]: 700mV 1200mV",
                                   "VDDC_CURVE_SCLK[1]: 810Mhz 2100Mhz",
                                   "VDDC_CURVE_VOLT[1]: 800mV 1300mV"};
    // clang-format on

    SECTION("Returns curve points")
    {
      auto values = ::Utils::AMD::parseOverdriveVoltCurveRange(input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      auto &[p0freq, p0volt] = values->at(0);
      auto &[p0fmin, p0fmax] = p0freq;
      REQUIRE(p0fmin == units::frequency::megahertz_t(800));
      REQUIRE(p0fmax == units::frequency::megahertz_t(2000));
      auto &[p0vmin, p0vmax] = p0volt;
      REQUIRE(p0vmin == units::voltage::millivolt_t(700));
      REQUIRE(p0vmax == units::voltage::millivolt_t(1200));

      auto &[p1freq, p1volt] = values->at(1);
      auto &[p1fmin, p1fmax] = p1freq;
      REQUIRE(p1fmin == units::frequency::megahertz_t(810));
      REQUIRE(p1fmax == units::frequency::megahertz_t(2100));
      auto &[p1vmin, p1vmax] = p1volt;
      REQUIRE(p1vmin == units::voltage::millivolt_t(800));
      REQUIRE(p1vmax == units::voltage::millivolt_t(1300));
    }

    SECTION("Returns nothing...")
    {
      SECTION("When input has no OD_RANGE")
      {
        std::vector<std::string> input{"OTHER:"};

        auto empty = ::Utils::AMD::parseOverdriveVoltRange(input);
        REQUIRE_FALSE(empty.has_value());
      }

      SECTION("When input has no point's range")
      {
        std::vector<std::string> input{"OD_RANGE:"};

        auto empty = ::Utils::AMD::parseOverdriveVoltRange(input);
        REQUIRE_FALSE(empty.has_value());
      }
    }

    SECTION("When input has missing data at least one point")
    {
      // clang-format off
        std::vector<std::string> missingFreqInput{"OD_RANGE:",
                                                  "...",
                                                  "VDDC_CURVE_VOLT[0]: 700mV 1200mV"};
        std::vector<std::string> missingVoltInput{"OD_RANGE:",
                                                  "...",
                                                  "VDDC_CURVE_SCLK[0]: 800Mhz 2000Mhz"};
      // clang-format on

      auto empty0 = ::Utils::AMD::parseOverdriveVoltRange(missingFreqInput);
      REQUIRE_FALSE(empty0.has_value());
      auto empty1 = ::Utils::AMD::parseOverdriveVoltRange(missingVoltInput);
      REQUIRE_FALSE(empty1.has_value());
    }
  }

  SECTION("parseOverdriveClkControls")
  {
    SECTION("Returns available CLK controls")
    {
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
                                     "...",
                                     "OD_MCLK:",
                                     "...",};
      // clang-format on

      auto values = ::Utils::AMD::parseOverdriveClkControls(input);
      REQUIRE(values.has_value());
      REQUIRE(values->size() == 2);

      REQUIRE(values->at(0) == "SCLK");
      REQUIRE(values->at(1) == "MCLK");
    }

    SECTION("Returns nothing when there is no available CLK controls")
    {
      std::vector<std::string> input{"OTHER_DATA"};

      auto empty = ::Utils::AMD::parseOverdriveClkControls(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("parseOverdriveVoltOffset")
  {
    SECTION("Returns voltage offset")
    {
      // clang-format off
      std::vector<std::string> input{"OD_VDDGFX_OFFSET:",
                                     "-10mV",
                                     "...",};
      // clang-format on

      auto offset = ::Utils::AMD::parseOverdriveVoltOffset(input);
      REQUIRE(offset.has_value());
      REQUIRE(*offset == units::voltage::millivolt_t(-10));
    }

    SECTION("Returns nothing when there is no available voltage offset")
    {
      std::vector<std::string> input{"OTHER_DATA"};

      auto empty = ::Utils::AMD::parseOverdriveVoltOffset(input);
      REQUIRE_FALSE(empty.has_value());
    }
  }

  SECTION("getOverdriveClkControlCmdId")
  {
    SECTION("Returns 's' command id for SCLK control")
    {
      auto cmdId = ::Utils::AMD::getOverdriveClkControlCmdId("SCLK");
      REQUIRE(cmdId == "s");
    }

    SECTION("Returns 'm' command id for MCLK control")
    {
      auto cmdId = ::Utils::AMD::getOverdriveClkControlCmdId("MCLK");
      REQUIRE(cmdId == "m");
    }
  }

  SECTION("ppOdClkVoltageHasKnownFreqVoltQuirks")
  {
    SECTION("Pre-Vega20 missing range section")
    {
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
                                     "0:        300MHz        800mV",
                                     "1:        608MHz        818mV"};
      // clang-format on
      REQUIRE(::Utils::AMD::ppOdClkVoltageHasKnownFreqVoltQuirks("SCLK", input));
    }

    SECTION("Good input has no quirks")
    {
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
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
      REQUIRE_FALSE(
          ::Utils::AMD::ppOdClkVoltageHasKnownFreqVoltQuirks("SCLK", input));
      REQUIRE_FALSE(
          ::Utils::AMD::ppOdClkVoltageHasKnownFreqVoltQuirks("MCLK", input));
    }
  }

  SECTION("ppOdClkVoltageHasKnownVoltCurveQuirks")
  {
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
      REQUIRE(::Utils::AMD::ppOdClkVoltageHasKnownVoltCurveQuirks(input));
    }

    SECTION("Good input has no quirks")
    {
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
      REQUIRE_FALSE(
          ::Utils::AMD::ppOdClkVoltageHasKnownVoltCurveQuirks(vega20Input));

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
      REQUIRE_FALSE(
          ::Utils::AMD::ppOdClkVoltageHasKnownVoltCurveQuirks(naviInput));
    }
  }

  SECTION("ppOdClkVoltageFreqRangeOutOfRangeState")
  {
    SECTION("Returns the out of range state indices")
    {
      // RX 6X00XT out of range minimum memory clock
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
                                     "0: 700Mhz",
                                     "1: 2629Mhz",
                                     "OD_MCLK:",
                                     "0: 97Mhz",
                                     "1: 1000MHz",
                                     "OD_VDDGFX_OFFSET:",
                                     "0mV",
                                     "OD_RANGE:",
                                     "SCLK:     500Mhz       3150Mhz",
                                     "MCLK:     674Mhz        1200Mhz"};
      // clang-format on
      auto states = ::Utils::AMD::ppOdClkVoltageFreqRangeOutOfRangeStates(
          "MCLK", input);

      REQUIRE(states.has_value());
      REQUIRE(states->size() == 1);
      REQUIRE(states->at(0) == 0);
    }

    SECTION("Returns nothing when there is no out of range states")
    {
      // clang-format off
      std::vector<std::string> input{"OD_SCLK:",
                                     "0: 700Mhz",
                                     "1: 2629Mhz",
                                     "OD_MCLK:",
                                     "0: 674Mhz",
                                     "1: 1000MHz",
                                     "OD_VDDGFX_OFFSET:",
                                     "0mV",
                                     "OD_RANGE:",
                                     "SCLK:     500Mhz       3150Mhz",
                                     "MCLK:     674Mhz        1200Mhz"};
      // clang-format on
      REQUIRE_FALSE(
          ::Utils::AMD::ppOdClkVoltageFreqRangeOutOfRangeStates("SCLK", input)
              .has_value());
      REQUIRE_FALSE(
          ::Utils::AMD::ppOdClkVoltageFreqRangeOutOfRangeStates("MCLK", input)
              .has_value());
    }
  }

  SECTION("hasOverdriveClkVoltControl")
  {
    SECTION("Returns true when overdrive has clock + voltage state controls")
    {
      // clang-format off
      std::vector<std::string> data{"OD_SCLK:",
                                    "0: 300MHz 800mV"};
      // clang-format on

      REQUIRE(::Utils::AMD::hasOverdriveClkVoltControl(data));
    }

    SECTION(
        "Returns false when overdrive has no clock + voltage state controls")
    {
      // clang-format off
      std::vector<std::string> otherClkControlData{"OD_SCLK:",
                                                   "0: 300MHz"};
      std::vector<std::string> noClkControlData{"OTHER_DATA"};
      // clang-format on

      REQUIRE_FALSE(
          ::Utils::AMD::hasOverdriveClkVoltControl(otherClkControlData));
      REQUIRE_FALSE(::Utils::AMD::hasOverdriveClkVoltControl(noClkControlData));
    }
  }

  SECTION("hasOverdriveClkControl")
  {
    SECTION("Returns true when overdrive has clock state controls")
    {
      // clang-format off
      std::vector<std::string> data{"OD_SCLK:",
                                    "0: 300MHz"};
      // clang-format on

      REQUIRE(::Utils::AMD::hasOverdriveClkControl(data));
    }

    SECTION("Returns false when overdrive has no clock state controls")
    {
      // clang-format off
      std::vector<std::string> otherClkControlData{"OD_SCLK:",
                                                   "0: 300MHz 800mV"};
      std::vector<std::string> noClkControlData{"OTHER_DATA"};
      // clang-format on

      REQUIRE_FALSE(::Utils::AMD::hasOverdriveClkControl(otherClkControlData));
      REQUIRE_FALSE(::Utils::AMD::hasOverdriveClkControl(noClkControlData));
    }
  }

  SECTION("hasOverdriveVoltCurveControl")
  {
    SECTION("Returns true when overdrive has voltage curve control")
    {
      std::vector<std::string> data{"OD_VDDC_CURVE:"};

      REQUIRE(::Utils::AMD::hasOverdriveVoltCurveControl(data));
    }

    SECTION("Returns false when overdrive has no voltage curve control")
    {
      std::vector<std::string> noClkControlData{"OTHER_DATA"};

      REQUIRE_FALSE(::Utils::AMD::hasOverdriveVoltCurveControl(noClkControlData));
    }
  }

  SECTION("hasOverdriveVoltOffsetControl")
  {
    SECTION("Returns true when overdrive has voltage offset control")
    {
      std::vector<std::string> data{"OD_VDDGFX_OFFSET:"};

      REQUIRE(::Utils::AMD::hasOverdriveVoltOffsetControl(data));
    }

    SECTION("Returns false when overdrive has no voltage curve control")
    {
      std::vector<std::string> noClkControlData{"OTHER_DATA"};

      REQUIRE_FALSE(
          ::Utils::AMD::hasOverdriveVoltOffsetControl(noClkControlData));
    }
  }
}
} // namespace AMD
} // namespace Utils
} // namespace Tests
