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
#include "common/ppdpmhandlermock.h"
#include "common/stringdatasourcestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/fvstate/pmfvstate.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFVState {

class PMFVStateTestAdapter : public ::AMD::PMFVState
{
 public:
  using ::AMD::PMFVState::PMFVState;

  using ::AMD::PMFVState::cleanControl;
  using ::AMD::PMFVState::exportControl;
  using ::AMD::PMFVState::gpuRange;
  using ::AMD::PMFVState::gpuState;
  using ::AMD::PMFVState::gpuStates;
  using ::AMD::PMFVState::gpuVoltMode;
  using ::AMD::PMFVState::importControl;
  using ::AMD::PMFVState::memRange;
  using ::AMD::PMFVState::memState;
  using ::AMD::PMFVState::memStates;
  using ::AMD::PMFVState::memVoltMode;
  using ::AMD::PMFVState::syncControl;
  using ::AMD::PMFVState::voltModes;
  using ::AMD::PMFVState::voltRange;
};

class PMFVStateImporterStub final : public ::AMD::PMFVState::Importer
{
 public:
  PMFVStateImporterStub(
      std::string const &gpuVoltMode, std::string const &memVoltMode,
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &gpuStates,
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &memStates,
      std::vector<unsigned int> const &gpuActiveStates,
      std::vector<unsigned int> const &memActiveStates)
  : gpuVoltMode_(gpuVoltMode)
  , memVoltMode_(memVoltMode)
  , gpuStates_(gpuStates)
  , memStates_(memStates)
  , gpuActiveStates_(gpuActiveStates)
  , memActiveStates_(memActiveStates)
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

  std::string const &providePMFVStateGPUVoltMode() const override
  {
    return gpuVoltMode_;
  }

  std::string const &providePMFVStateMemVoltMode() const override
  {
    return memVoltMode_;
  }

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateGPUState(unsigned int index) const override
  {
    auto stateIt = std::find_if(
        gpuStates_.cbegin(), gpuStates_.cend(),
        [=](auto &state) { return std::get<0>(state) == index; });

    return {std::get<1>(*stateIt), std::get<2>(*stateIt)};
  }

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateMemState(unsigned int index) const override
  {
    auto stateIt = std::find_if(
        memStates_.cbegin(), memStates_.cend(),
        [=](auto &state) { return std::get<0>(state) == index; });

    return {std::get<1>(*stateIt), std::get<2>(*stateIt)};
  }

  std::vector<unsigned int> providePMFVStateGPUActiveStates() const override
  {
    return gpuActiveStates_;
  }

  std::vector<unsigned int> providePMFVStateMemActiveStates() const override
  {
    return memActiveStates_;
  }

 private:
  std::string const gpuVoltMode_;
  std::string const memVoltMode_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>> const gpuStates_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>> const memStates_;
  std::vector<unsigned int> const gpuActiveStates_;
  std::vector<unsigned int> const memActiveStates_;
};

class PMFVStateExporterMock : public ::AMD::PMFVState::Exporter
{
 public:
  MAKE_MOCK1(takePMFVStateVoltModes, void(std::vector<std::string> const &),
             override);
  MAKE_MOCK1(takePMFVStateGPUVoltMode, void(std::string const &), override);
  MAKE_MOCK1(takePMFVStateMemVoltMode, void(std::string const &), override);

  MAKE_MOCK2(takePMFVStateVoltRange,
             void(units::voltage::millivolt_t, units::voltage::millivolt_t),
             override);
  MAKE_MOCK2(takePMFVStateGPURange,
             void(units::frequency::megahertz_t, units::frequency::megahertz_t),
             override);
  MAKE_MOCK2(takePMFVStateMemRange,
             void(units::frequency::megahertz_t, units::frequency::megahertz_t),
             override);

  MAKE_MOCK1(takePMFVStateGPUStates,
             void(std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                         units::voltage::millivolt_t>> const &),
             override);
  MAKE_MOCK1(takePMFVStateMemStates,
             void(std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                         units::voltage::millivolt_t>> const &),
             override);

  MAKE_MOCK1(takePMFVStateGPUActiveStates,
             void(std::vector<unsigned int> const &), override);
  MAKE_MOCK1(takePMFVStateMemActiveStates,
             void(std::vector<unsigned int> const &), override);

  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFVState tests", "[GPU][AMD][PM][PMAdvanced][PMFVState]")
{
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> sclkStates{
      {0, units::frequency::megahertz_t(300)},
      {1, units::frequency::megahertz_t(1000)},
      {2, units::frequency::megahertz_t(2000)}};
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> mclkStates{
      {0, units::frequency::megahertz_t(300)},
      {1, units::frequency::megahertz_t(2000)}};

  auto ppDpmSclkMock = std::make_unique<PpDpmHandlerMock>(sclkStates);
  auto ppDpmMclkMock = std::make_unique<PpDpmHandlerMock>(mclkStates);

  // clang-format off
  std::vector<std::string> ppOdClkVoltageData {
                             "OD_SCLK:",
                             "0:        200MHz        800mV",
                             "1:       1000MHz        850mV",
                             "2:       2000MHz        900mV",
                             "OD_MCLK:",
                             "0:        300MHz        800mV",
                             "1:       3000MHz        900mV",
                             "OD_RANGE:",
                             "SCLK:     200MHz       2000MHz",
                             "MCLK:     300MHz       3000MHz",
                             "VDDC:     800mV        900mV" };
  // clang-format on
  CommandQueueStub ctlCmds;

  SECTION("Has PMFVState ID")
  {
    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    REQUIRE(ts.ID() == ::AMD::PMFVState::ItemID);
  }

  SECTION("Is active by default")
  {
    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    REQUIRE(ts.active());
  }

  SECTION("Has 'auto' volt mode by default for gpu and memory")
  {
    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    REQUIRE(ts.gpuVoltMode() == "auto");
    REQUIRE(ts.memVoltMode() == "auto");
  }

  SECTION("Generate pre-init control commands")
  {
    REQUIRE_CALL(*ppDpmSclkMock, saveState());
    REQUIRE_CALL(*ppDpmMclkMock, saveState());
    REQUIRE_CALL(*ppDpmSclkMock, reset(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMclkMock, reset(trompeloeil::_));

    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(
                                "power_dpm_force_performance_level", "auto"),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
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
    REQUIRE_CALL(*ppDpmSclkMock, saveState());
    REQUIRE_CALL(*ppDpmMclkMock, saveState());
    REQUIRE_CALL(*ppDpmSclkMock, reset(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMclkMock, reset(trompeloeil::_));

    SECTION("To only restore power_dpm_force_performance_level value when it "
            "was not 'manual'")
    {
      PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(
                                  "power_dpm_force_performance_level", "auto"),
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData),
                              std::move(ppDpmSclkMock),
                              std::move(ppDpmMclkMock));
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
      REQUIRE_CALL(*ppDpmSclkMock, restoreState(trompeloeil::_));
      REQUIRE_CALL(*ppDpmMclkMock, restoreState(trompeloeil::_));

      PMFVStateTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
      ts.preInit(ctlCmds);
      ctlCmds.clear();
      ts.postInit(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 7);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "s 0 200 800");

      auto &[cmd2Path, cmd2Value] = commands.at(2);
      REQUIRE(cmd2Path == "pp_od_clk_voltage");
      REQUIRE(cmd2Value == "s 1 1000 850");

      auto &[cmd3Path, cmd3Value] = commands.at(3);
      REQUIRE(cmd3Path == "pp_od_clk_voltage");
      REQUIRE(cmd3Value == "s 2 2000 900");

      auto &[cmd4Path, cmd4Value] = commands.at(4);
      REQUIRE(cmd4Path == "pp_od_clk_voltage");
      REQUIRE(cmd4Value == "m 0 300 800");

      auto &[cmd5Path, cmd5Value] = commands.at(5);
      REQUIRE(cmd5Path == "pp_od_clk_voltage");
      REQUIRE(cmd5Value == "m 1 3000 900");

      auto &[cmd6Path, cmd6Value] = commands.at(6);
      REQUIRE(cmd6Path == "pp_od_clk_voltage");
      REQUIRE(cmd6Value == "c");
    }
  }

  SECTION("Initializes gpu/mem states and ranges from pp_od_clk_voltage data "
          "source")
  {
    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.init();

    auto gpuStates = ts.gpuStates();
    REQUIRE(gpuStates.size() == 3);

    auto &[s0Index, s0Freq, s0Volt] = gpuStates.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(200));
    REQUIRE(s0Volt == units::voltage::millivolt_t(800));

    auto &[s1Index, s1Freq, s1Volt] = gpuStates.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(1000));
    REQUIRE(s1Volt == units::voltage::millivolt_t(850));

    auto &[s2Index, s2Freq, s2Volt] = gpuStates.at(2);
    REQUIRE(s2Index == 2);
    REQUIRE(s2Freq == units::frequency::megahertz_t(2000));
    REQUIRE(s2Volt == units::voltage::millivolt_t(900));

    auto memStates = ts.memStates();
    REQUIRE(memStates.size() == 2);

    auto &[m0Index, m0Freq, m0Volt] = memStates.at(0);
    REQUIRE(m0Index == 0);
    REQUIRE(m0Freq == units::frequency::megahertz_t(300));
    REQUIRE(m0Volt == units::voltage::millivolt_t(800));

    auto &[m1Index, m1Freq, m1Volt] = memStates.at(1);
    REQUIRE(m1Index == 1);
    REQUIRE(m1Freq == units::frequency::megahertz_t(3000));
    REQUIRE(m1Volt == units::voltage::millivolt_t(900));

    auto &[gpuMin, gpuMax] = ts.gpuRange();
    REQUIRE(gpuMin == units::frequency::megahertz_t(200));
    REQUIRE(gpuMax == units::frequency::megahertz_t(2000));

    auto &[memMin, memMax] = ts.memRange();
    REQUIRE(memMin == units::frequency::megahertz_t(300));
    REQUIRE(memMax == units::frequency::megahertz_t(3000));

    auto &[voltMin, voltMax] = ts.voltRange();
    REQUIRE(voltMin == units::voltage::millivolt_t(800));
    REQUIRE(voltMax == units::voltage::millivolt_t(900));
  }

  SECTION("Clamps gpu state frequency and voltage values in range")
  {
    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.init();

    // mim
    ts.gpuState(0, units::frequency::megahertz_t(0),
                units::voltage::millivolt_t(0));
    // max
    ts.gpuState(1, units::frequency::megahertz_t(10000),
                units::voltage::millivolt_t(10000));

    auto states = ts.gpuStates();
    REQUIRE(states.size() == 3);

    // min
    auto &[s0Index, s0Freq, s0Volt] = states.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(200));
    REQUIRE(s0Volt == units::voltage::millivolt_t(800));

    // max
    auto &[s1Index, s1Freq, s1Volt] = states.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(2000));
    REQUIRE(s1Volt == units::voltage::millivolt_t(900));
  }

  SECTION("Clamps mem state frequency and voltage values in range")
  {
    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.init();

    // mim
    ts.memState(0, units::frequency::megahertz_t(0),
                units::voltage::millivolt_t(0));
    // max
    ts.memState(1, units::frequency::megahertz_t(10000),
                units::voltage::millivolt_t(10000));

    auto states = ts.memStates();
    REQUIRE(states.size() == 2);

    // min
    auto &[s0Index, s0Freq, s0Volt] = states.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(300));
    REQUIRE(s0Volt == units::voltage::millivolt_t(800));

    // max
    auto &[s1Index, s1Freq, s1Volt] = states.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(3000));
    REQUIRE(s1Volt == units::voltage::millivolt_t(900));
  }

  SECTION("Import its state")
  {
    std::vector<unsigned int> gpuActiveStates{0, 1};
    std::vector<unsigned int> memActiveStates{0};

    REQUIRE_CALL(*ppDpmSclkMock, activate(trompeloeil::_))
        .LR_WITH(_1 == gpuActiveStates);
    REQUIRE_CALL(*ppDpmMclkMock, activate(trompeloeil::_))
        .LR_WITH(_1 == memActiveStates);

    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.init();

    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        gpuStates;
    gpuStates.emplace_back(0, units::frequency::megahertz_t(201),
                           units::voltage::millivolt_t(801));
    gpuStates.emplace_back(1, units::frequency::megahertz_t(1001),
                           units::voltage::millivolt_t(851));
    gpuStates.emplace_back(2, units::frequency::megahertz_t(1000),
                           units::voltage::millivolt_t(800));

    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        memStates;
    memStates.emplace_back(0, units::frequency::megahertz_t(301),
                           units::voltage::millivolt_t(801));
    memStates.emplace_back(1, units::frequency::megahertz_t(2000),
                           units::voltage::millivolt_t(800));

    PMFVStateImporterStub i("manual", "manual", gpuStates, memStates,
                            gpuActiveStates, memActiveStates);

    ts.importControl(i);

    REQUIRE(ts.gpuVoltMode() == "manual");
    REQUIRE(ts.memVoltMode() == "manual");
    REQUIRE(ts.gpuStates() == gpuStates);
    REQUIRE(ts.memStates() == memStates);
  }

  SECTION("Export its state")
  {
    std::vector<unsigned int> gpuActiveStates{0, 1, 2};
    std::vector<unsigned int> memActiveStates{0, 1};

    ALLOW_CALL(*ppDpmSclkMock, active()).LR_RETURN(gpuActiveStates);
    ALLOW_CALL(*ppDpmMclkMock, active()).LR_RETURN(memActiveStates);

    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.init();

    std::vector<std::string> modes{"auto", "manual"};

    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        gpuStates;
    gpuStates.emplace_back(0, units::frequency::megahertz_t(200),
                           units::voltage::millivolt_t(800));
    gpuStates.emplace_back(1, units::frequency::megahertz_t(1000),
                           units::voltage::millivolt_t(850));
    gpuStates.emplace_back(2, units::frequency::megahertz_t(2000),
                           units::voltage::millivolt_t(900));

    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        memStates;
    memStates.emplace_back(0, units::frequency::megahertz_t(300),
                           units::voltage::millivolt_t(800));
    memStates.emplace_back(1, units::frequency::megahertz_t(3000),
                           units::voltage::millivolt_t(900));

    trompeloeil::sequence seq;
    PMFVStateExporterMock e;
    REQUIRE_CALL(e, takePMFVStateVoltModes(trompeloeil::_))
        .LR_WITH(_1 == modes)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateGPUVoltMode("auto")).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateMemVoltMode("auto")).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateGPURange(
                        trompeloeil::eq(units::frequency::megahertz_t(200)),
                        trompeloeil::eq(units::frequency::megahertz_t(2000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateMemRange(
                        trompeloeil::eq(units::frequency::megahertz_t(300)),
                        trompeloeil::eq(units::frequency::megahertz_t(3000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateVoltRange(
                        trompeloeil::eq(units::voltage::millivolt_t(800)),
                        trompeloeil::eq(units::voltage::millivolt_t(900))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateGPUStates(trompeloeil::_))
        .LR_WITH(_1 == gpuStates)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateMemStates(trompeloeil::_))
        .LR_WITH(_1 == memStates)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateGPUActiveStates(trompeloeil::_))
        .LR_WITH(_1 == gpuActiveStates)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFVStateMemActiveStates(trompeloeil::_))
        .LR_WITH(_1 == memActiveStates)
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Generate clean control commands unconditionally")
  {
    REQUIRE_CALL(*ppDpmSclkMock, reset(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMclkMock, reset(trompeloeil::_));

    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(
                                "power_dpm_force_performance_level", "manual"),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
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

  SECTION("Does not generate sync control commands when is synced")
  {
    REQUIRE_CALL(*ppDpmSclkMock, sync(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMclkMock, sync(trompeloeil::_));

    PMFVStateTestAdapter ts(std::make_unique<StringDataSourceStub>(
                                "power_dpm_force_performance_level", "manual"),
                            std::make_unique<VectorStringDataSourceStub>(
                                "pp_od_clk_voltage", ppOdClkVoltageData),
                            std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("power_dpm_force_performance_level is out of sync")
    {
      REQUIRE_CALL(*ppDpmSclkMock, apply(trompeloeil::_));
      REQUIRE_CALL(*ppDpmMclkMock, apply(trompeloeil::_));

      PMFVStateTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "_other_"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
      ts.init();
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 7);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "s 0 200 800");

      auto &[cmd2Path, cmd2Value] = commands.at(2);
      REQUIRE(cmd2Path == "pp_od_clk_voltage");
      REQUIRE(cmd2Value == "s 1 1000 850");

      auto &[cmd3Path, cmd3Value] = commands.at(3);
      REQUIRE(cmd3Path == "pp_od_clk_voltage");
      REQUIRE(cmd3Value == "s 2 2000 900");

      auto &[cmd4Path, cmd4Value] = commands.at(4);
      REQUIRE(cmd4Path == "pp_od_clk_voltage");
      REQUIRE(cmd4Value == "m 0 300 800");

      auto &[cmd5Path, cmd5Value] = commands.at(5);
      REQUIRE(cmd5Path == "pp_od_clk_voltage");
      REQUIRE(cmd5Value == "m 1 3000 900");

      auto &[cmd6Path, cmd6Value] = commands.at(6);
      REQUIRE(cmd6Path == "pp_od_clk_voltage");
      REQUIRE(cmd6Value == "c");
    }

    SECTION("gpu states are out of sync...")
    {
      REQUIRE_CALL(*ppDpmSclkMock, sync(trompeloeil::_));
      REQUIRE_CALL(*ppDpmMclkMock, sync(trompeloeil::_));

      PMFVStateTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
      ts.init();

      SECTION("and gpu volt mode is 'auto'")
      {
        ts.gpuVoltMode("auto");
        ts.gpuState(0, units::frequency::megahertz_t(201),
                    units::voltage::millivolt_t(801)); // not used in the command
        ts.syncControl(ctlCmds);

        auto &commands = ctlCmds.commands();
        REQUIRE(commands.size() == 2);

        auto &[cmd0Path, cmd0Value] = commands.at(0);
        REQUIRE(cmd0Path == "pp_od_clk_voltage");
        REQUIRE(cmd0Value == "s 0 201 800");

        auto &[cmd1Path, cmd1Value] = commands.at(1);
        REQUIRE(cmd1Path == "pp_od_clk_voltage");
        REQUIRE(cmd1Value == "c");
      }

      SECTION("and gpu volt mode is 'manual'")
      {
        ts.gpuVoltMode("manual");
        ts.gpuState(0, units::frequency::megahertz_t(201),
                    units::voltage::millivolt_t(801));
        ts.syncControl(ctlCmds);

        auto &commands = ctlCmds.commands();
        REQUIRE(commands.size() == 2);

        auto &[cmd0Path, cmd0Value] = commands.at(0);
        REQUIRE(cmd0Path == "pp_od_clk_voltage");
        REQUIRE(cmd0Value == "s 0 201 801");

        auto &[cmd1Path, cmd1Value] = commands.at(1);
        REQUIRE(cmd1Path == "pp_od_clk_voltage");
        REQUIRE(cmd1Value == "c");
      }
    }

    SECTION("memory states are out of sync...")
    {
      REQUIRE_CALL(*ppDpmSclkMock, sync(trompeloeil::_));
      REQUIRE_CALL(*ppDpmMclkMock, sync(trompeloeil::_));

      PMFVStateTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
      ts.init();

      SECTION("and memory volt mode is 'auto'")
      {
        ts.memVoltMode("auto");
        ts.memState(0, units::frequency::megahertz_t(301),
                    units::voltage::millivolt_t(801)); // not used in the command
        ts.syncControl(ctlCmds);

        auto &commands = ctlCmds.commands();
        REQUIRE(commands.size() == 2);

        auto &[cmd0Path, cmd0Value] = commands.at(0);
        REQUIRE(cmd0Path == "pp_od_clk_voltage");
        REQUIRE(cmd0Value == "m 0 301 800");

        auto &[cmd1Path, cmd1Value] = commands.at(1);
        REQUIRE(cmd1Path == "pp_od_clk_voltage");
        REQUIRE(cmd1Value == "c");
      }

      SECTION("and memory volt mode is 'manual'")
      {
        ts.memVoltMode("manual");
        ts.memState(0, units::frequency::megahertz_t(301),
                    units::voltage::millivolt_t(801));
        ts.syncControl(ctlCmds);

        auto &commands = ctlCmds.commands();
        REQUIRE(commands.size() == 2);

        auto &[cmd0Path, cmd0Value] = commands.at(0);
        REQUIRE(cmd0Path == "pp_od_clk_voltage");
        REQUIRE(cmd0Value == "m 0 301 801");

        auto &[cmd1Path, cmd1Value] = commands.at(1);
        REQUIRE(cmd1Path == "pp_od_clk_voltage");
        REQUIRE(cmd1Value == "c");
      }
    }
  }
}

} // namespace PMFVState
} // namespace AMD
} // namespace Tests
