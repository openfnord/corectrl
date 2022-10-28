// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/stringdatasourcestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/overdrive/voltoffset/pmvoltoffset.h"
#include "units/units.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMVoltOffset {

class PMVoltOffsetTestAdapter : public ::AMD::PMVoltOffset
{
 public:
  using ::AMD::PMVoltOffset::PMVoltOffset;

  using ::AMD::PMVoltOffset::cleanControl;
  using ::AMD::PMVoltOffset::exportControl;
  using ::AMD::PMVoltOffset::importControl;
  using ::AMD::PMVoltOffset::range;
  using ::AMD::PMVoltOffset::syncControl;
  using ::AMD::PMVoltOffset::value;
};

class PMVoltOffsetImporterStub final : public ::AMD::PMVoltOffset::Importer
{
 public:
  PMVoltOffsetImporterStub(units::voltage::millivolt_t const offset)
  : offset_(offset)
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

  units::voltage::millivolt_t providePMVoltOffsetValue() const override
  {
    return offset_;
  }

 private:
  units::voltage::millivolt_t const offset_;
};

class PMVoltOffsetExporterMock : public ::AMD::PMVoltOffset::Exporter
{
 public:
  MAKE_MOCK2(takePMVoltOffsetRange,
             void(units::voltage::millivolt_t, units::voltage::millivolt_t),
             override);
  MAKE_MOCK1(takePMVoltOffsetValue, void(units::voltage::millivolt_t), override);

  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMVoltOffset tests",
          "[GPU][AMD][PM][PMAdvanced][PMOverdrive][PMVoltOffset]")
{
  // clang-format off
  std::vector<std::string> ppOdClkVoltageData {
                             "OD_VDDGFX_OFFSET:",
                             "0mV"};
  // clang-format on
  CommandQueueStub ctlCmds;

  SECTION("Has PMVoltOffset ID")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.ID() == ::AMD::PMVoltOffset::ItemID);
  }

  SECTION("Is active by default")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.active());
  }

  SECTION("Does not generate pre-init control commands")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Generates post-init control commands")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.preInit(ctlCmds);
    ctlCmds.clear();
    ts.postInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "vo 0");
  }

  SECTION("Initializes offset from pp_od_clk_voltage data source")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    auto offset = ts.value();
    REQUIRE(offset == units::voltage::millivolt_t(0));
  }

  SECTION("Clamps offset value in range")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    auto range = ts.range();

    // min
    ts.value(units::voltage::millivolt_t(-1) + range.first);
    REQUIRE(ts.value() == range.first);

    // max
    ts.value(units::voltage::millivolt_t(1) + range.second);
    REQUIRE(ts.value() == range.second);
  }

  SECTION("Imports its state")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    auto offset = units::voltage::millivolt_t(-20);
    PMVoltOffsetImporterStub i(offset);

    ts.importControl(i);

    REQUIRE(ts.value() == offset);
  }

  SECTION("Exports its state")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    auto range = ts.range();

    trompeloeil::sequence seq;
    PMVoltOffsetExporterMock e;
    REQUIRE_CALL(e, takePMVoltOffsetRange(trompeloeil::eq(range.first),
                                          trompeloeil::eq(range.second)))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMVoltOffsetValue(units::voltage::millivolt_t(0)))
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Generates sync control commands when is out of sync")
  {
    PMVoltOffsetTestAdapter ts(std::make_unique<VectorStringDataSourceStub>(
        "pp_od_clk_voltage", ppOdClkVoltageData));
    ts.init();

    ts.value(units::voltage::millivolt_t(-20));
    ts.syncControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "vo -20");
  }
}

} // namespace PMVoltOffset
} // namespace AMD
} // namespace Tests
