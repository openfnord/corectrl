// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/ppdpmhandlermock.h"
#include "common/stringdatasourcestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/overdrive/freqvolt/pmfreqvolt.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFreqVolt {

class PMFreqVoltTestAdapter : public ::AMD::PMFreqVolt
{
 public:
  using ::AMD::PMFreqVolt::PMFreqVolt;

  using ::AMD::PMFreqVolt::cleanControl;
  using ::AMD::PMFreqVolt::controlCmdId;
  using ::AMD::PMFreqVolt::controlName;
  using ::AMD::PMFreqVolt::exportControl;
  using ::AMD::PMFreqVolt::freqRange;
  using ::AMD::PMFreqVolt::importControl;
  using ::AMD::PMFreqVolt::state;
  using ::AMD::PMFreqVolt::states;
  using ::AMD::PMFreqVolt::syncControl;
  using ::AMD::PMFreqVolt::voltMode;
  using ::AMD::PMFreqVolt::voltModes;
  using ::AMD::PMFreqVolt::voltRange;
};

class PMFreqVoltImporterStub final : public ::AMD::PMFreqVolt::Importer
{
 public:
  PMFreqVoltImporterStub(
      std::string const &voltMode,
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states,
      std::vector<unsigned int> const &activeStates)
  : voltMode_(voltMode)
  , states_(states)
  , activeStates_(activeStates)
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

  std::string const &providePMFreqVoltVoltMode() const override
  {
    return voltMode_;
  }

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFreqVoltState(unsigned int index) const override
  {
    auto stateIt = std::find_if(
        states_.cbegin(), states_.cend(),
        [=](auto &state) { return std::get<0>(state) == index; });

    return {std::get<1>(*stateIt), std::get<2>(*stateIt)};
  }

  std::vector<unsigned int> providePMFreqVoltActiveStates() const override
  {
    return activeStates_;
  }

 private:
  std::string const voltMode_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>> const states_;
  std::vector<unsigned int> const activeStates_;
};

class PMFreqVoltExporterMock : public ::AMD::PMFreqVolt::Exporter
{
 public:
  MAKE_MOCK1(takePMFreqVoltControlName, void(std::string const &), override);
  MAKE_MOCK1(takePMFreqVoltVoltModes, void(std::vector<std::string> const &),
             override);
  MAKE_MOCK1(takePMFreqVoltVoltMode, void(std::string const &), override);
  MAKE_MOCK2(takePMFreqVoltVoltRange,
             void(units::voltage::millivolt_t, units::voltage::millivolt_t),
             override);
  MAKE_MOCK2(takePMFreqVoltFreqRange,
             void(units::frequency::megahertz_t, units::frequency::megahertz_t),
             override);
  MAKE_MOCK1(takePMFreqVoltStates,
             void(std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                         units::voltage::millivolt_t>> const &),
             override);
  MAKE_MOCK1(takePMFreqVoltActiveStates,
             void(std::vector<unsigned int> const &), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFreqVolt tests",
          "[GPU][AMD][PM][PMAdvanced][PMOverdrive][PMFreqVolt]")
{
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states{
      {0, units::frequency::megahertz_t(300)},
      {1, units::frequency::megahertz_t(2000)}};

  auto ppDpmMock = std::make_unique<PpDpmHandlerMock>(states);

  // clang-format off
  std::vector<std::string> ppOdClkVoltageData {
                             "OD_MCLK:",
                             "0:        300MHz        800mV",
                             "1:       3000MHz        900mV",
                             "OD_RANGE:",
                             "MCLK:     300MHz       3000MHz",
                             "VDDC:     800mV        900mV" };
  // clang-format on
  CommandQueueStub ctlCmds;

  SECTION("Has PMFreqVolt ID")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(),
                             std::move(ppDpmMock));
    REQUIRE(ts.ID() == ::AMD::PMFreqVolt::ItemID);
  }

  SECTION("Is active by default")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(),
                             std::move(ppDpmMock));
    REQUIRE(ts.active());
  }

  SECTION("Has overdrive control name")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(),
                             std::move(ppDpmMock));
    REQUIRE(ts.controlName() == "MCLK");
  }

  SECTION("Has overdrive control command id")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(),
                             std::move(ppDpmMock));
    REQUIRE(ts.controlCmdId() == "m");
  }

  SECTION("Has 'auto' volt mode by default")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(),
                             std::move(ppDpmMock));
    REQUIRE(ts.voltMode() == "auto");
  }

  SECTION(
      "Resets dpm handler on pre-init but does not generates control commands")
  {
    REQUIRE_CALL(*ppDpmMock, saveState());
    REQUIRE_CALL(*ppDpmMock, reset(trompeloeil::_));

    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Generates post-init control commands")
  {
    REQUIRE_CALL(*ppDpmMock, saveState());
    REQUIRE_CALL(*ppDpmMock, reset(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMock, restoreState(trompeloeil::_));

    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.preInit(ctlCmds);
    ctlCmds.clear();
    ts.postInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 2);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "m 0 300 800");

    auto &[cmd1Path, cmd1Value] = commands.at(1);
    REQUIRE(cmd1Path == "pp_od_clk_voltage");
    REQUIRE(cmd1Value == "m 1 3000 900");
  }

  SECTION("Initializes states and range from pp_od_clk_voltage data source")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.init();

    auto states = ts.states();
    REQUIRE(states.size() == 2);

    auto &[s0Index, s0Freq, s0Volt] = states.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(300));
    REQUIRE(s0Volt == units::voltage::millivolt_t(800));

    auto &[s1Index, s1Freq, s1Volt] = states.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(3000));
    REQUIRE(s1Volt == units::voltage::millivolt_t(900));

    auto &[freqMin, freqMax] = ts.freqRange();
    REQUIRE(freqMin == units::frequency::megahertz_t(300));
    REQUIRE(freqMax == units::frequency::megahertz_t(3000));

    auto &[voltMin, voltMax] = ts.voltRange();
    REQUIRE(voltMin == units::voltage::millivolt_t(800));
    REQUIRE(voltMax == units::voltage::millivolt_t(900));
  }

  SECTION("Clamps state frequency and voltage values in range")
  {
    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.init();

    // min
    ts.state(0, units::frequency::megahertz_t(0), units::voltage::millivolt_t(0));
    // max
    ts.state(1, units::frequency::megahertz_t(10000),
             units::voltage::millivolt_t(10000));

    auto states = ts.states();
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

  SECTION("Imports its state")
  {
    std::vector<unsigned int> activeStates{0};
    REQUIRE_CALL(*ppDpmMock, activate(trompeloeil::_)).LR_WITH(_1 == activeStates);

    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.init();

    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        states;
    states.emplace_back(0, units::frequency::megahertz_t(301),
                        units::voltage::millivolt_t(801));
    states.emplace_back(1, units::frequency::megahertz_t(2000),
                        units::voltage::millivolt_t(800));

    PMFreqVoltImporterStub i("manual", states, activeStates);

    ts.importControl(i);

    REQUIRE(ts.voltMode() == "manual");
    REQUIRE(ts.states() == states);
  }

  SECTION("Exports its state")
  {
    std::vector<unsigned int> activeStates{0, 1};
    ALLOW_CALL(*ppDpmMock, active()).LR_RETURN(activeStates);

    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.init();

    std::vector<std::string> modes{"auto", "manual"};
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>>
        states;
    states.emplace_back(0, units::frequency::megahertz_t(300),
                        units::voltage::millivolt_t(800));
    states.emplace_back(1, units::frequency::megahertz_t(3000),
                        units::voltage::millivolt_t(900));

    trompeloeil::sequence seq;
    PMFreqVoltExporterMock e;
    REQUIRE_CALL(e, takePMFreqVoltControlName("MCLK")).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqVoltVoltModes(trompeloeil::_))
        .LR_WITH(_1 == modes)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqVoltVoltMode("auto")).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqVoltFreqRange(
                        trompeloeil::eq(units::frequency::megahertz_t(300)),
                        trompeloeil::eq(units::frequency::megahertz_t(3000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqVoltVoltRange(
                        trompeloeil::eq(units::voltage::millivolt_t(800)),
                        trompeloeil::eq(units::voltage::millivolt_t(900))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqVoltStates(trompeloeil::_))
        .LR_WITH(_1 == states)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqVoltActiveStates(trompeloeil::_))
        .LR_WITH(_1 == activeStates)
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Resets dpm handler but does not generates clean control commands")
  {
    REQUIRE_CALL(*ppDpmMock, reset(trompeloeil::_));

    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    REQUIRE_CALL(*ppDpmMock, sync(trompeloeil::_));

    PMFreqVoltTestAdapter ts("MCLK", "m",
                             std::make_unique<VectorStringDataSourceStub>(
                                 "pp_od_clk_voltage", ppOdClkVoltageData),
                             std::move(ppDpmMock));
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Generates sync control commands when...")
  {
    SECTION("states are out of sync...")
    {
      REQUIRE_CALL(*ppDpmMock, sync(trompeloeil::_));

      PMFreqVoltTestAdapter ts("MCLK", "m",
                               std::make_unique<VectorStringDataSourceStub>(
                                   "pp_od_clk_voltage", ppOdClkVoltageData),
                               std::move(ppDpmMock));
      ts.init();

      SECTION("and volt mode is 'auto'")
      {
        ts.voltMode("auto");
        ts.state(0, units::frequency::megahertz_t(301),
                 units::voltage::millivolt_t(801)); // not used in the command
        ts.syncControl(ctlCmds);

        auto &commands = ctlCmds.commands();
        REQUIRE(commands.size() == 1);

        auto &[cmd0Path, cmd0Value] = commands.at(0);
        REQUIRE(cmd0Path == "pp_od_clk_voltage");
        REQUIRE(cmd0Value == "m 0 301 800");
      }

      SECTION("and volt mode is 'manual'")
      {
        ts.voltMode("manual");
        ts.state(0, units::frequency::megahertz_t(301),
                 units::voltage::millivolt_t(801));
        ts.syncControl(ctlCmds);

        auto &commands = ctlCmds.commands();
        REQUIRE(commands.size() == 1);

        auto &[cmd0Path, cmd0Value] = commands.at(0);
        REQUIRE(cmd0Path == "pp_od_clk_voltage");
        REQUIRE(cmd0Value == "m 0 301 801");
      }
    }
  }
}

} // namespace PMFreqVolt
} // namespace AMD
} // namespace Tests
