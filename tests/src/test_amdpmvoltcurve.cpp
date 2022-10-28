// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/stringdatasourcestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/overdrive/voltcurve/pmvoltcurve.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMVoltCurve {

class PMVoltCurveTestAdapter : public ::AMD::PMVoltCurve
{
 public:
  using ::AMD::PMVoltCurve::PMVoltCurve;

  using ::AMD::PMVoltCurve::cleanControl;
  using ::AMD::PMVoltCurve::controlCmdId;
  using ::AMD::PMVoltCurve::exportControl;
  using ::AMD::PMVoltCurve::importControl;
  using ::AMD::PMVoltCurve::mode;
  using ::AMD::PMVoltCurve::modes;
  using ::AMD::PMVoltCurve::point;
  using ::AMD::PMVoltCurve::points;
  using ::AMD::PMVoltCurve::pointsRange;
  using ::AMD::PMVoltCurve::syncControl;
};

class PMVoltCurveImporterStub final : public ::AMD::PMVoltCurve::Importer
{
 public:
  PMVoltCurveImporterStub(
      std::string const &mode,
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const points)
  : mode_(mode)
  , points_(points)
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

  std::string const &providePMVoltCurveMode() const override
  {
    return mode_;
  }

  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMVoltCurvePoint(unsigned int index) const override
  {
    return points_[index];
  }

 private:
  std::string const mode_;
  std::vector<std::pair<units::frequency::megahertz_t,
                        units::voltage::millivolt_t>> const points_;
};

class PMVoltCurveExporterMock : public ::AMD::PMVoltCurve::Exporter
{
 public:
  MAKE_MOCK1(takePMVoltCurveModes, void(std::vector<std::string> const &),
             override);
  MAKE_MOCK1(takePMVoltCurveMode, void(std::string const &), override);
  MAKE_MOCK1(
      takePMVoltCurvePointsRange,
      void(std::vector<std::pair<
               std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
               std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
               &),
      override);
  MAKE_MOCK1(takePMVoltCurvePoints,
             void(std::vector<std::pair<units::frequency::megahertz_t,
                                        units::voltage::millivolt_t>> const &),
             override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMVoltCurve tests",
          "[GPU][AMD][PM][PMAdvanced][PMOverdrive][PMVoltCurve]")
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

  SECTION("Has PMVoltCurve ID")
  {
    PMVoltCurveTestAdapter ts("vc",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.ID() == ::AMD::PMVoltCurve::ItemID);
  }

  SECTION("Is active by default")
  {
    PMVoltCurveTestAdapter ts("vc",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.active());
  }

  SECTION("Has 'auto' mode by default")
  {
    PMVoltCurveTestAdapter ts("vc",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.mode() == "auto");
  }

  SECTION("Has overdrive control command id")
  {
    PMVoltCurveTestAdapter ts("vc",
                              std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.controlCmdId() == "vc");
  }

  SECTION("Does not generate pre-init control commands")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Generate post-init control commands")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.preInit(ctlCmds);
    ctlCmds.clear();
    ts.postInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 3);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "vc 0 200 800");

    auto &[cmd1Path, cmd1Value] = commands.at(1);
    REQUIRE(cmd1Path == "pp_od_clk_voltage");
    REQUIRE(cmd1Value == "vc 1 1000 850");

    auto &[cmd2Path, cmd2Value] = commands.at(2);
    REQUIRE(cmd2Path == "pp_od_clk_voltage");
    REQUIRE(cmd2Value == "vc 2 2000 900");
  }

  SECTION("Initializes points and range from pp_od_clk_voltage data source")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();

    auto &pointsRange = ts.pointsRange();
    REQUIRE(pointsRange.size() == 3);

    auto &[p0RangeFreq, p0RangeVolt] = pointsRange.at(0);
    REQUIRE(p0RangeFreq.first == units::frequency::megahertz_t(200));
    REQUIRE(p0RangeFreq.second == units::frequency::megahertz_t(2000));
    REQUIRE(p0RangeVolt.first == units::voltage::millivolt_t(800));
    REQUIRE(p0RangeVolt.second == units::voltage::millivolt_t(900));

    auto &[p1RangeFreq, p1RangeVolt] = pointsRange.at(1);
    REQUIRE(p1RangeFreq.first == units::frequency::megahertz_t(200));
    REQUIRE(p1RangeFreq.second == units::frequency::megahertz_t(2000));
    REQUIRE(p1RangeVolt.first == units::voltage::millivolt_t(800));
    REQUIRE(p1RangeVolt.second == units::voltage::millivolt_t(900));

    auto &[p2RangeFreq, p2RangeVolt] = pointsRange.at(2);
    REQUIRE(p2RangeFreq.first == units::frequency::megahertz_t(200));
    REQUIRE(p2RangeFreq.second == units::frequency::megahertz_t(2000));
    REQUIRE(p2RangeVolt.first == units::voltage::millivolt_t(800));
    REQUIRE(p2RangeVolt.second == units::voltage::millivolt_t(900));

    auto &points = ts.points();
    auto &[p0Freq, p0Volt] = points.at(0);
    REQUIRE(p0Freq == units::frequency::megahertz_t(200));
    REQUIRE(p0Volt == units::voltage::millivolt_t(800));

    auto &[p1Freq, p1Volt] = points.at(1);
    REQUIRE(p1Freq == units::frequency::megahertz_t(1000));
    REQUIRE(p1Volt == units::voltage::millivolt_t(850));

    auto &[p2Freq, p2Volt] = points.at(2);
    REQUIRE(p2Freq == units::frequency::megahertz_t(2000));
    REQUIRE(p2Volt == units::voltage::millivolt_t(900));
  }

  SECTION("Clamps point values in range")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();

    // min
    ts.point(0, units::frequency::megahertz_t(0), units::voltage::millivolt_t(0));
    // max
    ts.point(1, units::frequency::megahertz_t(10000),
             units::voltage::millivolt_t(10000));

    auto curve = ts.points();
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

  SECTION("Imports its state")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();

    std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
        points;
    points.emplace_back(units::frequency::megahertz_t(201),
                        units::voltage::millivolt_t(801));
    points.emplace_back(units::frequency::megahertz_t(1001),
                        units::voltage::millivolt_t(851));
    points.emplace_back(units::frequency::megahertz_t(1000),
                        units::voltage::millivolt_t(850));

    PMVoltCurveImporterStub i("manual", points);

    ts.importControl(i);

    REQUIRE(ts.mode() == "manual");
    REQUIRE(ts.points() == points);
  }

  SECTION("Exports its state")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();

    std::vector<std::string> modes{"auto", "manual"};

    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
        pointsRange;
    for (int i = 0; i < 3; ++i)
      pointsRange.emplace_back(
          std::make_pair(units::frequency::megahertz_t(200),
                         units::frequency::megahertz_t(2000)),
          std::make_pair(units::voltage::millivolt_t(800),
                         units::voltage::millivolt_t(900)));

    std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
        points;
    points.emplace_back(units::frequency::megahertz_t(200),
                        units::voltage::millivolt_t(800));
    points.emplace_back(units::frequency::megahertz_t(1000),
                        units::voltage::millivolt_t(850));
    points.emplace_back(units::frequency::megahertz_t(2000),
                        units::voltage::millivolt_t(900));

    trompeloeil::sequence seq;
    PMVoltCurveExporterMock e;
    REQUIRE_CALL(e, takePMVoltCurveModes(trompeloeil::_))
        .LR_WITH(_1 == modes)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMVoltCurveMode("auto")).IN_SEQUENCE(seq);

    REQUIRE_CALL(e, takePMVoltCurvePointsRange(trompeloeil::_))
        .LR_WITH(_1 == pointsRange)
        .IN_SEQUENCE(seq);

    REQUIRE_CALL(e, takePMVoltCurvePoints(trompeloeil::_))
        .LR_WITH(_1 == points)
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {

    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Generates sync control commands when points are out of sync")
  {
    PMVoltCurveTestAdapter ts(
        "vc", std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                           ppOdClkVoltageData));
    ts.init();

    ts.mode("manual");
    ts.point(0, units::frequency::megahertz_t(201),
             units::voltage::millivolt_t(801));
    ts.syncControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "vc 0 201 801");
  }
}

} // namespace PMVoltCurve
} // namespace AMD
} // namespace Tests
