// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/uintdatasourcestub.h"
#include "core/components/controls/amd/fan/fixed/fanfixed.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace FanFixed {

class FanFixedTestAdapter : public ::AMD::FanFixed
{
 public:
  using ::AMD::FanFixed::FanFixed;

  using ::AMD::FanFixed::cleanControl;
  using ::AMD::FanFixed::exportControl;
  using ::AMD::FanFixed::fanStartValue;
  using ::AMD::FanFixed::fanStop;
  using ::AMD::FanFixed::importControl;
  using ::AMD::FanFixed::syncControl;
  using ::AMD::FanFixed::value;
};

class FanFixedImporterStub : public ::AMD::FanFixed::Importer
{
 public:
  FanFixedImporterStub()
  : value_(0)
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

  units::concentration::percent_t provideFanFixedValue() const override
  {
    return value_;
  }

  bool provideFanFixedFanStop() const override
  {
    return fanStop_;
  }

  units::concentration::percent_t provideFanFixedFanStartValue() const override
  {
    return fanStartValue_;
  }

  void value(units::concentration::percent_t value)
  {
    value_ = value;
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
  units::concentration::percent_t value_;
  units::concentration::percent_t fanStartValue_;
  bool fanStop_;
};

class FanFixedExporterMock : public ::AMD::FanFixed::Exporter
{
 public:
  MAKE_MOCK1(takeFanFixedValue, void(units::concentration::percent_t), override);
  MAKE_MOCK1(takeFanFixedFanStop, void(bool), override);
  MAKE_MOCK1(takeFanFixedFanStartValue, void(units::concentration::percent_t),
             override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD FanFixed tests", "[GPU][AMD][Fan][FanFixed]")
{
  CommandQueueStub ctlCmds;

  SECTION("Has FanFixed ID")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    REQUIRE(ts.ID() == ::AMD::FanFixed::ItemID);
  }

  SECTION("Is not active by default")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    REQUIRE_FALSE(ts.active());
  }

  SECTION("Has value == 64 (25%) by default")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    REQUIRE(ts.value() == 64);
  }

  SECTION("Clamp its value in [0, 255] range")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    ts.value(300);
    REQUIRE(ts.value() == 255);
  }

  SECTION("Has fan stop disabled by default")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    REQUIRE_FALSE(ts.fanStop());
  }

  SECTION("Has fan start value == 54 (21%) by default")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    REQUIRE(ts.fanStartValue() == 54);
  }

  SECTION("Clamp its fan start value in [0, 255] range")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());

    ts.fanStartValue(300);
    REQUIRE(ts.fanStartValue() == 255);
  }

  SECTION("Does not generate pre-init control commands")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());
    ts.preInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());
    ts.postInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Imports its state")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());
    FanFixedImporterStub i;
    i.value(100);
    i.fanStartValue(50);
    i.fanStop(true);

    ts.importWith(i);

    REQUIRE(ts.value() == 255);
    REQUIRE(ts.fanStartValue() == 128);
    REQUIRE(ts.fanStop());
  }

  SECTION("Export its state")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());
    ts.value(128);
    ts.fanStop(true);
    ts.fanStartValue(128);
    units::concentration::percent_t percentValue(50);

    FanFixedExporterMock e;
    REQUIRE_CALL(e, takeFanFixedValue(trompeloeil::_)).LR_WITH(_1 == percentValue);
    REQUIRE_CALL(e, takeFanFixedFanStop(true));
    REQUIRE_CALL(e, takeFanFixedFanStartValue(trompeloeil::_))
        .LR_WITH(_1 == percentValue);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>());
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>("pwm_enabled", 1),
                           std::make_unique<UIntDataSourceStub>("pwm", 128));
    ts.value(128);

    ts.syncControl(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("pwm_enabled is out of sync")
    {
      FanFixedTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm_enabled", 0),
          std::make_unique<UIntDataSourceStub>("pwm", 128));
      ts.value(128);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == "pwm_enabled");
      REQUIRE(cmd0Value == "1");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == "pwm");
      REQUIRE(cmd1Value == "128");
    }

    SECTION("pwm is out of sync")
    {
      FanFixedTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm_enabled", 1),
          std::make_unique<UIntDataSourceStub>("pwm", 0));
      ts.value(128);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);

      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pwm");
      REQUIRE(cmdValue == "128");
    }

    SECTION("pwm_enabled and pwm are out of sync")
    {
      FanFixedTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pwm_enabled", 0),
          std::make_unique<UIntDataSourceStub>("pwm", 0));
      ts.value(128);

      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == "pwm_enabled");
      REQUIRE(cmd0Value == "1");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == "pwm");
      REQUIRE(cmd1Value == "128");
    }
  }

  SECTION("Synced hw value uses fan start value when enabled")
  {
    FanFixedTestAdapter ts(std::make_unique<UIntDataSourceStub>("pwm_enabled", 1),
                           std::make_unique<UIntDataSourceStub>("pwm", 20));
    ts.value(20);
    ts.fanStartValue(30);
    ts.fanStop(false);

    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());

    ts.fanStop(true);
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);
    auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
    REQUIRE(cmdPath == "pwm");
    REQUIRE(cmdValue == "0");
  }
}
} // namespace FanFixed
} // namespace AMD
} // namespace Tests
