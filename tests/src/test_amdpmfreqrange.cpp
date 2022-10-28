// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/overdrive/freqrange/pmfreqrange.h"
#include "units/units.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFreqRange {

class PMFreqRangeTestAdapter : public ::AMD::PMFreqRange
{
 public:
  using ::AMD::PMFreqRange::PMFreqRange;

  using ::AMD::PMFreqRange::cleanControl;
  using ::AMD::PMFreqRange::controlCmdId;
  using ::AMD::PMFreqRange::controlName;
  using ::AMD::PMFreqRange::exportControl;
  using ::AMD::PMFreqRange::importControl;
  using ::AMD::PMFreqRange::state;
  using ::AMD::PMFreqRange::stateRange;
  using ::AMD::PMFreqRange::states;
  using ::AMD::PMFreqRange::syncControl;
};

class PMFreqRangeImporterStub final : public ::AMD::PMFreqRange::Importer
{
 public:
  PMFreqRangeImporterStub(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states)
  : states_(states)
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

  units::frequency::megahertz_t
  providePMFreqRangeState(unsigned int index) const override
  {
    auto stateIt = std::find_if(
        states_.cbegin(), states_.cend(),
        [=](auto &state) { return state.first == index; });

    return stateIt->second;
  }

 private:
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const states_;
};

class PMFreqRangeExporterMock : public ::AMD::PMFreqRange::Exporter
{
 public:
  MAKE_MOCK1(takePMFreqRangeControlName, void(std::string const &), override);
  MAKE_MOCK2(takePMFreqRangeStateRange,
             void(units::frequency::megahertz_t, units::frequency::megahertz_t),
             override);
  MAKE_MOCK1(
      takePMFreqRangeStates,
      void(std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &),
      override);

  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFreqRange tests",
          "[GPU][AMD][PM][PMAdvanced][PMOverdrive][PMFreqRange]")
{
  // clang-format off
  std::vector<std::string> ppOdClkVoltageData {
                             "OD_SCLK:",
                             "0:        200MHz",
                             "1:       1000MHz",
                             "OD_RANGE:",
                             "SCLK:     200MHz       2000MHz"};
  // clang-format on
  CommandQueueStub ctlCmds;

  SECTION("Has PMFreqRange ID")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.ID() == ::AMD::PMFreqRange::ItemID);
  }

  SECTION("Is active by default")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.active());
  }

  SECTION("Has overdrive control name")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.controlName() == "SCLK");
  }

  SECTION("Has overdrive control command id")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.controlCmdId() == "s");
  }

  SECTION("Does not generate pre-init control commands")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Generates post-init control commands")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.preInit(ctlCmds);
    ctlCmds.clear();
    ts.postInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 2);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "s 0 200");

    auto &[cmd1Path, cmd1Value] = commands.at(1);
    REQUIRE(cmd1Path == "pp_od_clk_voltage");
    REQUIRE(cmd1Value == "s 1 1000");
  }

  SECTION(
      "Does not generate post-init control commands for disabled bound states")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData),
                              ::AMD::PMFreqRange::DisabledBound{1});
    ts.preInit(ctlCmds);
    ctlCmds.clear();
    ts.postInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "s 0 200");
  }

  SECTION("Initializes states and range from pp_od_clk_voltage data source")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    auto states = ts.states();
    REQUIRE(states.size() == 2);

    auto &[s0Index, s0Freq] = states.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(200));

    auto &[s1Index, s1Freq] = states.at(1);
    REQUIRE(s1Index == 1);
    REQUIRE(s1Freq == units::frequency::megahertz_t(1000));

    auto &[min, max] = ts.stateRange();
    REQUIRE(min == units::frequency::megahertz_t(200));
    REQUIRE(max == units::frequency::megahertz_t(2000));
  }

  SECTION("Initializes states and range from pp_od_clk_voltage data source, "
          "skipping disabled bound states")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData),
                              ::AMD::PMFreqRange::DisabledBound{1});
    ts.init();

    auto states = ts.states();
    REQUIRE(states.size() == 1);

    auto &[s0Index, s0Freq] = states.at(0);
    REQUIRE(s0Index == 0);
    REQUIRE(s0Freq == units::frequency::megahertz_t(200));

    auto &[min, max] = ts.stateRange();
    REQUIRE(min == units::frequency::megahertz_t(200));
    REQUIRE(max == units::frequency::megahertz_t(2000));
  }

  SECTION("Clamps state frequency value in range")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    // min
    ts.state(0, units::frequency::megahertz_t(0));
    // max
    ts.state(1, units::frequency::megahertz_t(10000));

    auto states = ts.states();
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

  SECTION("Imports its state")
  {
    std::vector<unsigned int> gpuActiveStates{0, 1};
    std::vector<unsigned int> memActiveStates{1};

    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;
    states.emplace_back(0, units::frequency::megahertz_t(201));
    states.emplace_back(1, units::frequency::megahertz_t(1001));

    PMFreqRangeImporterStub i(states);

    ts.importControl(i);

    REQUIRE(ts.states() == states);
  }

  SECTION("Exports its state")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;
    states.emplace_back(0, units::frequency::megahertz_t(200));
    states.emplace_back(1, units::frequency::megahertz_t(1000));

    trompeloeil::sequence seq;
    PMFreqRangeExporterMock e;
    REQUIRE_CALL(e, takePMFreqRangeControlName(trompeloeil::_))
        .LR_WITH(_1 == "SCLK")
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqRangeStateRange(
                        trompeloeil::eq(units::frequency::megahertz_t(200)),
                        trompeloeil::eq(units::frequency::megahertz_t(2000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqRangeStates(trompeloeil::_))
        .LR_WITH(_1 == states)
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Generates sync control commands when is out of sync")
  {
    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    ts.state(0, units::frequency::megahertz_t(201));
    ts.syncControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "s 0 201");
  }

  SECTION("Generated sync control commands ignores disabled bound state")
  {
    // clang-format off
    std::vector<std::string> ppOdClkVoltageData {
                               "OD_SCLK:",
                               "0:        10MHz", // Bogus state that will be disabled
                               "1:       1000MHz",
                               "OD_RANGE:",
                               "SCLK:     200MHz       2000MHz"};
    // clang-format on

    PMFreqRangeTestAdapter ts("SCLK", "s",
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData),
                              ::AMD::PMFreqRange::DisabledBound{0});
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }
}

} // namespace PMFreqRange
} // namespace AMD
} // namespace Tests
