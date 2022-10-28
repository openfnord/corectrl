// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/uintdatasourcestub.h"
#include "core/components/controls/amd/fan/curve/fancurve.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace FanCurve {

class FanCurveTestAdapter : public ::AMD::FanCurve
{
 public:
  using ::AMD::FanCurve::FanCurve;

  using ::AMD::FanCurve::cleanControl;
  using ::AMD::FanCurve::curve;
  using ::AMD::FanCurve::evaluatePwm;
  using ::AMD::FanCurve::evaluateTemp;
  using ::AMD::FanCurve::exportControl;
  using ::AMD::FanCurve::fanStartValue;
  using ::AMD::FanCurve::fanStop;
  using ::AMD::FanCurve::hysteresis;
  using ::AMD::FanCurve::importControl;
  using ::AMD::FanCurve::lerpFromPwm;
  using ::AMD::FanCurve::lerpFromTemp;
  using ::AMD::FanCurve::syncControl;
};

class FanCurveImporterStub : public ::AMD::FanCurve::Importer
{
 public:
  FanCurveImporterStub(std::vector<::AMD::FanCurve::Point> const &curve)
  : curve_(curve)
  , fanStartValue_(0)
  , fanStop_(false)
  {
  }

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return *this;
  }

  bool provideActive() const override
  {
    return false;
  }

  std::vector<::AMD::FanCurve::Point> const &provideFanCurvePoints() const override
  {
    return curve_;
  }

  bool provideFanCurveFanStop() const override
  {
    return fanStop_;
  }

  units::concentration::percent_t provideFanCurveFanStartValue() const override
  {
    return fanStartValue_;
  }

  void fanStop(bool enabled)
  {
    fanStop_ = enabled;
  }

  void fanStartValue(units::concentration::percent_t value)
  {
    fanStartValue_ = value;
  }

 private:
  std::vector<::AMD::FanCurve::Point> const curve_;
  units::concentration::percent_t fanStartValue_;
  bool fanStop_;
};

class FanCurveExporterMock : public ::AMD::FanCurve::Exporter
{
 public:
  MAKE_MOCK1(takeFanCurvePoints,
             void(std::vector<::AMD::FanCurve::Point> const &), override);
  MAKE_MOCK1(takeFanCurveFanStop, void(bool), override);
  MAKE_MOCK1(takeFanCurveFanStartValue, void(units::concentration::percent_t),
             override);
  MAKE_MOCK2(takeFanCurveTemperatureRange,
             void(units::temperature::celsius_t, units::temperature::celsius_t),
             override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

class IntDataSourceStub : public IDataSource<int>
{
 public:
  IntDataSourceStub(std::string_view source = "", int data = 0,
                    bool success = true) noexcept
  : source_(source)
  , data_(data)
  , success_(success)
  {
  }

  std::string source() const override
  {
    return source_;
  }

  bool read(int &data) override
  {
    data = data_;
    return success_;
  }

  void data(int data)
  {
    data_ = data;
  }

  std::string const source_;
  int data_;
  bool success_;
};

TEST_CASE("AMD FanCurve tests", "[GPU][AMD][Fan][FanCurve]")
{
  CommandQueueStub ctlCmds;

  SECTION("Pwm curve evaluation")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    auto p1 = std::make_pair(units::temperature::celsius_t(0),
                             units::concentration::percent_t(0));
    auto p2 = std::make_pair(units::temperature::celsius_t(100),
                             units::concentration::percent_t(100));

    REQUIRE(ts.lerpFromPwm(units::concentration::percent_t(50), p1, p2) == 50);

    // out of line input
    REQUIRE(ts.lerpFromPwm(units::concentration::percent_t(-50), p1, p2) == 0);
    REQUIRE(ts.lerpFromPwm(units::concentration::percent_t(150), p1, p2) == 100);

    std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
        curve;
    curve.push_back(p1);
    curve.push_back(p2);
    ts.curve(curve);

    REQUIRE(ts.evaluatePwm(units::concentration::percent_t(50)) == 50);

    // out of curve pwm
    REQUIRE(ts.evaluatePwm(units::concentration::percent_t(-50)) == 0);
    REQUIRE(ts.evaluatePwm(units::concentration::percent_t(150)) == 100);
  }

  SECTION("Temperature curve evaluation")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    auto p1 = std::make_pair(units::temperature::celsius_t(0),
                             units::concentration::percent_t(0));
    auto p2 = std::make_pair(units::temperature::celsius_t(100),
                             units::concentration::percent_t(100));

    REQUIRE(ts.lerpFromTemp(units::temperature::celsius_t(50), p1, p2) == 128);

    // out of line input
    REQUIRE(ts.lerpFromTemp(units::temperature::celsius_t(-50), p1, p2) == 0);
    REQUIRE(ts.lerpFromTemp(units::temperature::celsius_t(150), p1, p2) == 255);

    std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
        curve;
    curve.push_back(p1);
    curve.push_back(p2);
    ts.curve(curve);

    REQUIRE(ts.evaluateTemp(units::temperature::celsius_t(50)) == 128);

    // out of curve temperatures
    REQUIRE(ts.evaluateTemp(units::temperature::celsius_t(-50)) == 0);
    REQUIRE(ts.evaluateTemp(units::temperature::celsius_t(150)) == 255);
  }

  SECTION("Has FanCurve ID")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    REQUIRE(ts.ID() == ::AMD::FanCurve::ItemID);
  }

  SECTION("Is not active by default")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    REQUIRE_FALSE(ts.active());
  }

  SECTION("Has a default curve")
  {
    auto tempRange = std::make_pair(units::temperature::celsius_t(0),
                                    units::temperature::celsius_t(50));

    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           tempRange.first, tempRange.second);

    REQUIRE(ts.curve().size() > 2);

    SECTION("Default curve points are normalized into temperature range")
    {
      REQUIRE_FALSE(
          std::any_of(ts.curve().cbegin(), ts.curve().cend(), [&](auto &point) {
            return point.first < tempRange.first ||
                   point.first > tempRange.second;
          }));
    }
  }

  SECTION("Computed pwm1 value is clampled in [0, 255] range")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    auto p1 = std::make_pair(units::temperature::celsius_t(0),
                             units::concentration::percent_t(0));
    auto p2 = std::make_pair(units::temperature::celsius_t(100),
                             units::concentration::percent_t(100));

    REQUIRE(ts.lerpFromTemp(units::temperature::celsius_t(-50), p1, p2) == 0);
    REQUIRE(ts.lerpFromTemp(units::temperature::celsius_t(50), p1, p2) == 128);
    REQUIRE(ts.lerpFromTemp(units::temperature::celsius_t(150), p1, p2) == 255);
  }

  SECTION("Has fan stop disabled by default")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    REQUIRE_FALSE(ts.fanStop());
  }

  SECTION("Has hysteresis > 0")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    REQUIRE(ts.hysteresis() > 0);
  }

  SECTION("Has fan start value == 54 (21%) by default")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    REQUIRE(ts.fanStartValue() == 54);
  }

  SECTION("Clamp its fan start value in [0, 255] range")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    ts.fanStartValue(300);
    REQUIRE(ts.fanStartValue() == 255);
  }

  SECTION("Does not generate pre-init control commands")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));
    ts.preInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));
    ts.postInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Imports its state")
  {
    auto p1 = std::make_pair(units::temperature::celsius_t(0),
                             units::concentration::percent_t(0));
    auto p2 = std::make_pair(units::temperature::celsius_t(200), // out of range
                             units::concentration::percent_t(200)); // out of range
    std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
        curve;
    curve.push_back(p1);
    curve.push_back(p2);

    FanCurveImporterStub i(curve);
    i.fanStartValue(50);
    i.fanStop(true);

    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));

    ts.importControl(i);

    auto normalizedCurve = curve;
    auto &lastPoint = normalizedCurve.back();
    lastPoint.first = units::temperature::celsius_t(100);
    lastPoint.second = units::concentration::percent_t(100);

    REQUIRE_FALSE(ts.curve().empty());
    REQUIRE(ts.curve() == normalizedCurve);
    REQUIRE(ts.fanStartValue() == 128);
    REQUIRE(ts.fanStop());
  }

  SECTION("Export its state")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));
    auto p1 = std::make_pair(units::temperature::celsius_t(0),
                             units::concentration::percent_t(0));
    auto p2 = std::make_pair(units::temperature::celsius_t(100),
                             units::concentration::percent_t(100));
    std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
        curve;
    curve.push_back(p1);
    curve.push_back(p2);
    ts.curve(curve);
    ts.fanStop(true);
    ts.fanStartValue(128);

    trompeloeil::sequence seq;
    FanCurveExporterMock e;
    REQUIRE_CALL(e, takeFanCurveTemperatureRange(trompeloeil::_, trompeloeil::_))
        .WITH(_1 == units::temperature::celsius_t(0))
        .WITH(_2 == units::temperature::celsius_t(100))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takeFanCurvePoints(trompeloeil::_))
        .LR_WITH(_1 == curve)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takeFanCurveFanStop(true)).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takeFanCurveFanStartValue(trompeloeil::_))
        .WITH(_1 == units::concentration::percent_t(50))
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    FanCurveTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<IntDataSourceStub>(),
                           units::temperature::celsius_t(0),
                           units::temperature::celsius_t(100));
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when...")
  {
    SECTION("is synced")
    {
      FanCurveTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
          std::make_unique<UIntDataSourceStub>("pwm1", 128),
          std::make_unique<IntDataSourceStub>("temp1_input", 50),
          units::temperature::celsius_t(0), units::temperature::celsius_t(100));
      auto p1 = std::make_pair(units::temperature::celsius_t(0),
                               units::concentration::percent_t(0));
      auto p2 = std::make_pair(units::temperature::celsius_t(100),
                               units::concentration::percent_t(100));
      std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
          curve;
      curve.push_back(p1);
      curve.push_back(p2);
      ts.curve(curve);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().empty());
    }
  }

  SECTION("Does generate sync control commands when...")
  {
    auto p1 = std::make_pair(units::temperature::celsius_t(0),
                             units::concentration::percent_t(0));
    auto p2 = std::make_pair(units::temperature::celsius_t(100),
                             units::concentration::percent_t(100));
    std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
        curve;
    curve.push_back(p1);
    curve.push_back(p2);

    SECTION("pwm1_enabled is out of sync")
    {
      FanCurveTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm1_enabled", 0),
          std::make_unique<UIntDataSourceStub>("pwm1", 128),
          std::make_unique<IntDataSourceStub>("temp1_input", 50),
          units::temperature::celsius_t(0), units::temperature::celsius_t(100));
      ts.curve(curve);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == "pwm1_enabled");
      REQUIRE(cmd0Value == "1");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == "pwm1");
      REQUIRE(cmd1Value == "128");
    }

    SECTION("pwm1_enabled and pwm1 are out of sync")
    {
      FanCurveTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm1_enabled", 0),
          std::make_unique<UIntDataSourceStub>("pwm1", 12),
          std::make_unique<IntDataSourceStub>("temp1_input", 50),
          units::temperature::celsius_t(0), units::temperature::celsius_t(100));
      ts.curve(curve);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == "pwm1_enabled");
      REQUIRE(cmd0Value == "1");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == "pwm1");
      REQUIRE(cmd1Value == "128");
    }

    SECTION("fan stop is disabled and pwm1 is out of sync")
    {
      FanCurveTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
          std::make_unique<UIntDataSourceStub>("pwm1", 12),
          std::make_unique<IntDataSourceStub>("temp1_input", 50),
          units::temperature::celsius_t(0), units::temperature::celsius_t(100));
      ts.curve(curve);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);

      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pwm1");
      REQUIRE(cmdValue == "128");
    }

    SECTION("fan stop is enabled and...")
    {
      SECTION("temperature is >= fan start temperature and pwm1 is out of sync")
      {
        auto tempDataSourcePtr = std::make_unique<IntDataSourceStub>(
            "temp1_input", 0);
        auto &tempDataSource = *tempDataSourcePtr;
        FanCurveTestAdapter ts(
            std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
            std::make_unique<UIntDataSourceStub>("pwm1", 40),
            std::move(tempDataSourcePtr), units::temperature::celsius_t(0),
            units::temperature::celsius_t(100));
        ts.curve(curve);
        ts.fanStop(true);
        ts.fanStartValue(128); // 50% -> start temperature = 50 degrees C

        tempDataSource.data(50);
        ts.syncControl(ctlCmds);

        REQUIRE(ctlCmds.commands().size() == 1);
        auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
        REQUIRE(cmdPath == "pwm1");
        REQUIRE(cmdValue == "128");
      }

      SECTION("temperature is fanStartTemp - hyst and pwm1 > 0 but out of sync")
      {
        auto tempDataSourcePtr = std::make_unique<IntDataSourceStub>(
            "temp1_input", 0);
        auto &tempDataSource = *tempDataSourcePtr;
        FanCurveTestAdapter ts(
            std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
            std::make_unique<UIntDataSourceStub>("pwm1", 154),
            std::move(tempDataSourcePtr), units::temperature::celsius_t(0),
            units::temperature::celsius_t(100));
        ts.curve(curve);
        ts.fanStop(true);
        ts.fanStartValue(128); // 50% -> start temperature = 50 degrees C

        // temperature equals to fan start - hysteresis
        tempDataSource.data(50 - ts.hysteresis());
        ts.syncControl(ctlCmds);

        REQUIRE(ctlCmds.commands().size() == 1);
        auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
        REQUIRE(cmdPath == "pwm1");
        REQUIRE(cmdValue == "128"); // value of fan start temperature
      }

      SECTION("temperature is in (fanStartTemp - hyst, fanStartTemp) range "
              "and pwm1 > 0 but out of sync")
      {
        auto tempDataSourcePtr = std::make_unique<IntDataSourceStub>(
            "temp1_input", 0);
        auto &tempDataSource = *tempDataSourcePtr;
        FanCurveTestAdapter ts(
            std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
            std::make_unique<UIntDataSourceStub>("pwm1", 154),
            std::move(tempDataSourcePtr), units::temperature::celsius_t(0),
            units::temperature::celsius_t(100));
        ts.curve(curve);
        ts.fanStop(true);
        ts.fanStartValue(128); // 50% -> start temperature = 50 degrees C

        // temperature in (fan start - hysteresis, fan start) range
        tempDataSource.data(49);
        ts.syncControl(ctlCmds);

        REQUIRE(ctlCmds.commands().size() == 1);
        auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
        REQUIRE(cmdPath == "pwm1");
        REQUIRE(cmdValue == "128"); // value of fan start temperature
      }

      SECTION("temperature is < fanStartTemp - hyst and pwm1 > 0")
      {
        auto tempDataSourcePtr = std::make_unique<IntDataSourceStub>(
            "temp1_input", 0);
        auto &tempDataSource = *tempDataSourcePtr;
        FanCurveTestAdapter ts(
            std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
            std::make_unique<UIntDataSourceStub>("pwm1", 128),
            std::move(tempDataSourcePtr), units::temperature::celsius_t(0),
            units::temperature::celsius_t(100));
        ts.curve(curve);
        ts.fanStop(true);
        ts.fanStartValue(128); // 50% -> start temperature = 50 degrees C

        // temperature lower than fan start - hysteresis
        tempDataSource.data(50 - ts.hysteresis() - 1);
        ts.syncControl(ctlCmds);

        REQUIRE(ctlCmds.commands().size() == 1);
        auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
        REQUIRE(cmdPath == "pwm1");
        REQUIRE(cmdValue == "0"); // turns off fan
      }

      SECTION("fan stop was enabled and then disabled")
      {
        FanCurveTestAdapter ts(
            std::make_unique<UIntDataSourceStub>("pwm1_enabled", 1),
            std::make_unique<UIntDataSourceStub>("pwm1", 0),
            std::make_unique<IntDataSourceStub>("temp1_input", 50),
            units::temperature::celsius_t(0),
            units::temperature::celsius_t(100));
        ts.curve(curve);
        ts.fanStop(true);

        ts.syncControl(ctlCmds);
        ctlCmds = {};

        ts.fanStop(false);
        ts.syncControl(ctlCmds);

        REQUIRE(ctlCmds.commands().size() == 1);
        auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
        REQUIRE(cmdPath == "pwm1");
        REQUIRE(cmdValue != "0");
      }
    }
  }
}
} // namespace FanCurve
} // namespace AMD
} // namespace Tests
