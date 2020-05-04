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
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/stringdatasourcestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/fvvoltcurve/pmfvvoltcurve.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFVVoltCurve {

class PMFVVoltCurveTestAdapter : public ::AMD::PMFVVoltCurve
{
 public:
  using ::AMD::PMFVVoltCurve::PMFVVoltCurve;

  using ::AMD::PMFVVoltCurve::cleanControl;
  using ::AMD::PMFVVoltCurve::exportControl;
  using ::AMD::PMFVVoltCurve::gpuRange;
  using ::AMD::PMFVVoltCurve::gpuState;
  using ::AMD::PMFVVoltCurve::gpuStates;
  using ::AMD::PMFVVoltCurve::importControl;
  using ::AMD::PMFVVoltCurve::memRange;
  using ::AMD::PMFVVoltCurve::memState;
  using ::AMD::PMFVVoltCurve::memStates;
  using ::AMD::PMFVVoltCurve::syncControl;
  using ::AMD::PMFVVoltCurve::voltCurve;
  using ::AMD::PMFVVoltCurve::voltCurvePoint;
  using ::AMD::PMFVVoltCurve::voltMode;
  using ::AMD::PMFVVoltCurve::voltModes;
  using ::AMD::PMFVVoltCurve::voltRange;
};

class PMFVVoltCurveImporterStub final : public ::AMD::PMFVVoltCurve::Importer
{
 public:
  PMFVVoltCurveImporterStub(
      std::string const &voltMode,
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const voltCurve,
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &gpuStates,
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &memStates)
  : voltMode_(voltMode)
  , voltCurve_(voltCurve)
  , gpuStates_(gpuStates)
  , memStates_(memStates)
  {
  }

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return {};
  }

  bool provideActive() const override
  {
    return false;
  }

  std::string const &providePMFVVoltCurveVoltMode() const override
  {
    return voltMode_;
  }

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVVoltCurveVoltCurvePoint(unsigned int index) const override
  {
    return voltCurve_[index];
  }

  units::frequency::megahertz_t
  providePMFVVoltCurveGPUState(unsigned int index) const override
  {
    auto stateIt = std::find_if(
        gpuStates_.cbegin(), gpuStates_.cend(),
        [=](auto &state) { return state.first == index; });

    return stateIt->second;
  }

  units::frequency::megahertz_t
  providePMFVVoltCurveMemState(unsigned int index) const override
  {
    auto stateIt = std::find_if(
        memStates_.cbegin(), memStates_.cend(),
        [=](auto &state) { return state.first == index; });

    return stateIt->second;
  }

 private:
  std::string const voltMode_;
  std::vector<std::pair<units::frequency::megahertz_t,
                        units::voltage::millivolt_t>> const voltCurve_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const gpuStates_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const memStates_;
};

class PMFVVoltCurveExporterMock : public ::AMD::PMFVVoltCurve::Exporter
{
 public:
  MAKE_MOCK1(takePMFVVoltCurveVoltModes, void(std::vector<std::string> const &),
             override);
  MAKE_MOCK1(takePMFVVoltCurveVoltMode, void(std::string const &), override);

  MAKE_MOCK1(
      takePMFVVoltCurveVoltRange,
      void(std::vector<std::pair<
               std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
               std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
               &),
      override);
  MAKE_MOCK2(takePMFVVoltCurveGPURange,
             void(units::frequency::megahertz_t, units::frequency::megahertz_t),
             override);
  MAKE_MOCK2(takePMFVVoltCurveMemRange,
             void(units::frequency::megahertz_t, units::frequency::megahertz_t),
             override);

  MAKE_MOCK1(takePMFVVoltCurveVoltCurve,
             void(std::vector<std::pair<units::frequency::megahertz_t,
                                        units::voltage::millivolt_t>> const &),
             override);

  MAKE_MOCK1(
      takePMFVVoltCurveGPUStates,
      void(std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &),
      override);
  MAKE_MOCK1(
      takePMFVVoltCurveMemStates,
      void(std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &),
      override);

  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFVVoltCurve tests",
          "[GPU][AMD][PM][PMAdvanced][PMFVVoltCurve]")
{
  // clang-format off
  std::vector<std::string> ppOdClkVoltageData {
                             "OD_SCLK:",
                             "0:        200MHz",
                             "1:       1000MHz",
                             "OD_MCLK:",
                             "1:       1000MHz",
                             "OD_VDDC_CURVE:",
                             "0:        200MHz        800mV",
                             "1:       1000MHz        850mV",
                             "2:       2000MHz        900mV",
                             "OD_RANGE:",
                             "SCLK:     200MHz       2000MHz",
                             "MCLK:     300MHz       3000MHz",
                             "VDDC_CURVE_SCLK[0]:     200MHz        2000MHz",
                             "VDDC_CURVE_VOLT[0]:     800mV         900mV",
                             "VDDC_CURVE_SCLK[1]:     200MHz        2000MHz",
                             "VDDC_CURVE_VOLT[1]:     800mV         900mV",
                             "VDDC_CURVE_SCLK[2]:     200MHz        2000MHz",
                             "VDDC_CURVE_VOLT[2]:     800mV         900mV" };
  // clang-format on
  CommandQueueStub ctlCmds;

  SECTION("Has PMFVVoltCurve ID")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.ID() == ::AMD::PMFVVoltCurve::ItemID);
  }

  SECTION("Is active by default")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.active());
  }

  SECTION("Has 'auto' volt mode by default")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.voltMode() == "auto");
  }

  SECTION("Generate pre-init control commands")
  {
    PMFVVoltCurveTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "auto"),
        std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                     ppOdClkVoltageData));
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 3);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "power_dpm_force_performance_level");
    REQUIRE(cmd0Value == "manual");

    auto &[cmd1Path, cmd1Value] = commands.at(1);
    REQUIRE(cmd1Path == "pp_od_clk_voltage");
    REQUIRE(cmd1Value == "r");

    auto &[cmd2Path, cmd2Value] = commands.at(2);
    REQUIRE(cmd2Path == "pp_od_clk_voltage");
    REQUIRE(cmd2Value == "c");
  }

  SECTION("Generate post-init control commands...")
  {
    SECTION("To only restore power_dpm_force_performance_level value when it "
            "was not 'manual'")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "auto"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.preInit(ctlCmds);
      ctlCmds.clear();
      ts.postInit(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 1);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "auto");
    }

    SECTION("To restore reseted values when power_dpm_force_performance_level "
            "value was 'manual'")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.preInit(ctlCmds);
      ctlCmds.clear();
      ts.postInit(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 8);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "s 0 200");

      auto &[cmd2Path, cmd2Value] = commands.at(2);
      REQUIRE(cmd2Path == "pp_od_clk_voltage");
      REQUIRE(cmd2Value == "s 1 1000");

      auto &[cmd3Path, cmd3Value] = commands.at(3);
      REQUIRE(cmd3Path == "pp_od_clk_voltage");
      REQUIRE(cmd3Value == "m 1 1000");

      auto &[cmd4Path, cmd4Value] = commands.at(4);
      REQUIRE(cmd4Path == "pp_od_clk_voltage");
      REQUIRE(cmd4Value == "vc 0 200 800");

      auto &[cmd5Path, cmd5Value] = commands.at(5);
      REQUIRE(cmd5Path == "pp_od_clk_voltage");
      REQUIRE(cmd5Value == "vc 1 1000 850");

      auto &[cmd6Path, cmd6Value] = commands.at(6);
      REQUIRE(cmd6Path == "pp_od_clk_voltage");
      REQUIRE(cmd6Value == "vc 2 2000 900");

      auto &[cmd7Path, cmd7Value] = commands.at(7);
      REQUIRE(cmd7Path == "pp_od_clk_voltage");
      REQUIRE(cmd7Value == "c");
    }
  }

  SECTION("Initializes gpu/mem states, ranges and voltage curve from "
          "pp_od_clk_voltage data source")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>(
                                    "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    auto gpuStates = ts.gpuStates();
    REQUIRE(gpuStates.size() == 2);

    auto &[s0Index, s0Freq] = gpuStates.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(200));

    auto &[s1Index, s1Freq] = gpuStates.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(1000));

    auto memStates = ts.memStates();
    REQUIRE(memStates.size() == 1);

    auto &[m0Index, m0Freq] = memStates.at(0);
    REQUIRE(m0Index == 1);
    REQUIRE(m0Freq == units::frequency::megahertz_t(1000));

    auto &[gpuMin, gpuMax] = ts.gpuRange();
    REQUIRE(gpuMin == units::frequency::megahertz_t(200));
    REQUIRE(gpuMax == units::frequency::megahertz_t(2000));

    auto &[memMin, memMax] = ts.memRange();
    REQUIRE(memMin == units::frequency::megahertz_t(300));
    REQUIRE(memMax == units::frequency::megahertz_t(3000));

    auto &voltRange = ts.voltRange();
    REQUIRE(voltRange.size() == 3);

    auto &[p0Freq, p0Volt] = voltRange.at(0);
    REQUIRE(p0Freq.first == units::frequency::megahertz_t(200));
    REQUIRE(p0Freq.second == units::frequency::megahertz_t(2000));
    REQUIRE(p0Volt.first == units::voltage::millivolt_t(800));
    REQUIRE(p0Volt.second == units::voltage::millivolt_t(900));

    auto &[p1Freq, p1Volt] = voltRange.at(1);
    REQUIRE(p1Freq.first == units::frequency::megahertz_t(200));
    REQUIRE(p1Freq.second == units::frequency::megahertz_t(2000));
    REQUIRE(p1Volt.first == units::voltage::millivolt_t(800));
    REQUIRE(p1Volt.second == units::voltage::millivolt_t(900));

    auto &[p2Freq, p2Volt] = voltRange.at(2);
    REQUIRE(p2Freq.first == units::frequency::megahertz_t(200));
    REQUIRE(p2Freq.second == units::frequency::megahertz_t(2000));
    REQUIRE(p2Volt.first == units::voltage::millivolt_t(800));
    REQUIRE(p2Volt.second == units::voltage::millivolt_t(900));
  }

  SECTION("Clamps gpu state frequency value in range")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>(
                                    "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    // min
    ts.gpuState(0, units::frequency::megahertz_t(0));
    // max
    ts.gpuState(1, units::frequency::megahertz_t(10000));

    auto states = ts.gpuStates();
    REQUIRE(states.size() == 2);

    // min
    auto &[s0Index, s0Freq] = states.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(200));

    // max
    auto &[s1Index, s1Freq] = states.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(2000));
  }

  SECTION("Clamps mem state frequency value in range")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>(
                                    "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    // min
    ts.memState(1, units::frequency::megahertz_t(0));

    auto states = ts.memStates();
    REQUIRE(states.size() == 1);

    auto &[minIndex, minFreq] = states.at(0);
    REQUIRE(minIndex == 1);
    REQUIRE(minFreq == units::frequency::megahertz_t(300));

    // max
    ts.memState(1, units::frequency::megahertz_t(10000));

    states = ts.memStates();
    REQUIRE(states.size() == 1);

    auto &[maxIndex, maxFreq] = states.at(0);
    REQUIRE(maxIndex == 1);
    REQUIRE(maxFreq == units::frequency::megahertz_t(3000));
  }

  SECTION("Clamps volt point values in range")
  {
    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>(
                                    "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    // mim
    ts.voltCurvePoint(0, units::frequency::megahertz_t(0),
                      units::voltage::millivolt_t(0));
    // max
    ts.voltCurvePoint(1, units::frequency::megahertz_t(10000),
                      units::voltage::millivolt_t(10000));

    auto curve = ts.voltCurve();
    REQUIRE(curve.size() == 3);

    // min
    auto &[p0Freq, p0Volt] = curve.at(0);
    REQUIRE(p0Freq == units::frequency::megahertz_t(200));
    REQUIRE(p0Volt == units::voltage::millivolt_t(800));

    // max
    auto &[p1Freq, p1Volt] = curve.at(1);
    REQUIRE(p1Freq == units::frequency::megahertz_t(2000));
    REQUIRE(p1Volt == units::voltage::millivolt_t(900));
  }

  SECTION("Import its state")
  {
    std::vector<unsigned int> gpuActiveStates{0, 1};
    std::vector<unsigned int> memActiveStates{1};

    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>(
                                    "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuStates;
    gpuStates.emplace_back(0, units::frequency::megahertz_t(201));
    gpuStates.emplace_back(1, units::frequency::megahertz_t(1001));

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memStates;
    memStates.emplace_back(1, units::frequency::megahertz_t(2000));

    std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
        voltCurve;
    voltCurve.emplace_back(units::frequency::megahertz_t(201),
                           units::voltage::millivolt_t(801));
    voltCurve.emplace_back(units::frequency::megahertz_t(1001),
                           units::voltage::millivolt_t(851));
    voltCurve.emplace_back(units::frequency::megahertz_t(1000),
                           units::voltage::millivolt_t(850));

    PMFVVoltCurveImporterStub i("manual", voltCurve, gpuStates, memStates);

    ts.importControl(i);

    REQUIRE(ts.voltMode() == "manual");
    REQUIRE(ts.gpuStates() == gpuStates);
    REQUIRE(ts.memStates() == memStates);
    REQUIRE(ts.voltCurve() == voltCurve);
  }

  SECTION("Export its state")
  {
    std::vector<unsigned int> gpuActiveStates{0, 1, 2};
    std::vector<unsigned int> memActiveStates{1};

    PMFVVoltCurveTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<VectorStringDataSourceStub>(
                                    "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    std::vector<std::string> modes{"auto", "manual"};

    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
        curveRange;
    for (int i = 0; i < 3; ++i)
      curveRange.emplace_back(std::make_pair(units::frequency::megahertz_t(200),
                                             units::frequency::megahertz_t(2000)),
                              std::make_pair(units::voltage::millivolt_t(800),
                                             units::voltage::millivolt_t(900)));

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuStates;
    gpuStates.emplace_back(0, units::frequency::megahertz_t(200));
    gpuStates.emplace_back(1, units::frequency::megahertz_t(1000));

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memStates;
    memStates.emplace_back(1, units::frequency::megahertz_t(1000));

    std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
        voltCurve;
    voltCurve.emplace_back(units::frequency::megahertz_t(200),
                           units::voltage::millivolt_t(800));
    voltCurve.emplace_back(units::frequency::megahertz_t(1000),
                           units::voltage::millivolt_t(850));
    voltCurve.emplace_back(units::frequency::megahertz_t(2000),
                           units::voltage::millivolt_t(900));

    trompeloeil::sequence seq;
    PMFVVoltCurveExporterMock e;
    REQUIRE_CALL(e, takePMFVVoltCurveVoltModes(trompeloeil::_))
        .LR_WITH(_1 == modes)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVVoltCurveVoltMode("auto")).IN_SEQUENCE(seq);

    REQUIRE_CALL(e, takePMFVVoltCurveGPURange(
                        trompeloeil::eq(units::frequency::megahertz_t(200)),
                        trompeloeil::eq(units::frequency::megahertz_t(2000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVVoltCurveMemRange(
                        trompeloeil::eq(units::frequency::megahertz_t(300)),
                        trompeloeil::eq(units::frequency::megahertz_t(3000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVVoltCurveVoltRange(trompeloeil::_))
        .LR_WITH(_1 == curveRange)
        .IN_SEQUENCE(seq);

    REQUIRE_CALL(e, takePMFVVoltCurveVoltCurve(trompeloeil::_))
        .LR_WITH(_1 == voltCurve)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVVoltCurveGPUStates(trompeloeil::_))
        .LR_WITH(_1 == gpuStates)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVVoltCurveMemStates(trompeloeil::_))
        .LR_WITH(_1 == memStates)
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Generate clean control commands unconditionally")
  {
    PMFVVoltCurveTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "manual"),
        std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                     ppOdClkVoltageData));
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 3);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "power_dpm_force_performance_level");
    REQUIRE(cmd0Value == "manual");

    auto &[cmd1Path, cmd1Value] = commands.at(1);
    REQUIRE(cmd1Path == "pp_od_clk_voltage");
    REQUIRE(cmd1Value == "r");

    auto &[cmd2Path, cmd2Value] = commands.at(2);
    REQUIRE(cmd2Path == "pp_od_clk_voltage");
    REQUIRE(cmd2Value == "c");
  }

  SECTION("Does not generate sync control commands when...")
  {
    SECTION("is synced")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.init();
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().empty());
    }

    SECTION("voltage mode is 'auto' and voltage curve is out of sync")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.init();

      ts.voltMode("auto");
      ts.voltCurvePoint(0, units::frequency::megahertz_t(201),
                        units::voltage::millivolt_t(801));
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().empty());
    }
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("power_dpm_force_performance_level is out of sync")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "_other_"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.init();
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 8);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "s 0 200");

      auto &[cmd2Path, cmd2Value] = commands.at(2);
      REQUIRE(cmd2Path == "pp_od_clk_voltage");
      REQUIRE(cmd2Value == "s 1 1000");

      auto &[cmd3Path, cmd3Value] = commands.at(3);
      REQUIRE(cmd3Path == "pp_od_clk_voltage");
      REQUIRE(cmd3Value == "m 1 1000");

      auto &[cmd4Path, cmd4Value] = commands.at(4);
      REQUIRE(cmd4Path == "pp_od_clk_voltage");
      REQUIRE(cmd4Value == "vc 0 200 800");

      auto &[cmd5Path, cmd5Value] = commands.at(5);
      REQUIRE(cmd5Path == "pp_od_clk_voltage");
      REQUIRE(cmd5Value == "vc 1 1000 850");

      auto &[cmd6Path, cmd6Value] = commands.at(6);
      REQUIRE(cmd6Path == "pp_od_clk_voltage");
      REQUIRE(cmd6Value == "vc 2 2000 900");

      auto &[cmd7Path, cmd7Value] = commands.at(7);
      REQUIRE(cmd7Path == "pp_od_clk_voltage");
      REQUIRE(cmd7Value == "c");
    }

    SECTION("gpu states are out of sync")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.init();

      ts.gpuState(0, units::frequency::megahertz_t(201));
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 2);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "pp_od_clk_voltage");
      REQUIRE(cmd0Value == "s 0 201");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "c");
    }

    SECTION("memory states are out of sync")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.init();

      ts.memState(1, units::frequency::megahertz_t(301));
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 2);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "pp_od_clk_voltage");
      REQUIRE(cmd0Value == "m 1 301");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "c");
    }

    SECTION("voltage mode is 'manual' and voltage curve is out of sync")
    {
      PMFVVoltCurveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData));
      ts.init();

      ts.voltMode("manual");
      ts.voltCurvePoint(0, units::frequency::megahertz_t(201),
                        units::voltage::millivolt_t(801));
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 2);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "pp_od_clk_voltage");
      REQUIRE(cmd0Value == "vc 0 201 801");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "c");
    }
  }
}

} // namespace PMFVVoltCurve
} // namespace AMD
} // namespace Tests
