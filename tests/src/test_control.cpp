// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "core/components/controls/control.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace Control {

class ControlMock : public ::Control
{
 public:
  ControlMock(bool active = true, bool forceClean = false)
  : ::Control(active, forceClean)
  {
  }

  MAKE_MOCK1(preInit, void(ICommandQueue &), override);
  MAKE_MOCK1(postInit, void(ICommandQueue &), override);
  MAKE_MOCK0(init, void(), override);
  MAKE_MOCK1(importControl, void(IControl::Importer &), override);
  MAKE_CONST_MOCK1(exportControl, void(IControl::Exporter &), override);
  MAKE_MOCK1(cleanControl, void(ICommandQueue &), override);
  MAKE_MOCK1(syncControl, void(ICommandQueue &), override);
  MAKE_CONST_MOCK0(ID, std::string const &(), override);
};

class ControlImporterStub : public IControl::Importer
{
 public:
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return *this;
  }

  bool provideActive() const override
  {
    return false;
  }
};

class ControlExporterMock : public IControl::Exporter
{
 public:
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("Control tests", "[GPU][Control]")
{
  ControlMock ts;
  std::string const tsID("_id_");
  ALLOW_CALL(ts, ID()).LR_RETURN(tsID);

  SECTION("Is active by default")
  {
    REQUIRE(ts.active());

    SECTION("Can be deactivated")
    {
      ts.activate(false);

      REQUIRE_FALSE(ts.active());

      SECTION("Can be activated")
      {
        ts.activate(true);

        REQUIRE(ts.active());
      }
    }
  }

  SECTION("Imports active state and control")
  {
    REQUIRE_CALL(ts, importControl(trompeloeil::_));

    ControlImporterStub i;
    ts.importWith(i);

    REQUIRE_FALSE(ts.active());
  }

  SECTION("Exports active state and control")
  {
    REQUIRE_CALL(ts, exportControl(trompeloeil::_));

    ControlExporterMock e;
    ALLOW_CALL(e, provideExporter(trompeloeil::_)).LR_RETURN(e);
    REQUIRE_CALL(e, takeActive(ts.active()));

    ts.exportWith(e);
  }

  SECTION("Cleans control when gone from active to inactive")
  {
    CommandQueueStub ctlCmds;

    REQUIRE_CALL(ts, cleanControl(trompeloeil::_));
    ts.activate(true);
    ts.activate(false);
    ts.clean(ctlCmds);

    FORBID_CALL(ts, cleanControl(trompeloeil::_));
    ts.activate(true);
    ts.clean(ctlCmds);
  }

  SECTION("Cleans control when force clean is true")
  {
    ControlMock tsForceClean(false, true);
    CommandQueueStub ctlCmds;

    REQUIRE_CALL(tsForceClean, cleanControl(trompeloeil::_));
    tsForceClean.clean(ctlCmds);
  }

  SECTION("Cleans control when clean once is requested")
  {
    CommandQueueStub ctlCmds;

    REQUIRE_CALL(ts, cleanControl(trompeloeil::_));
    ts.cleanOnce();
    ts.clean(ctlCmds);
  }

  SECTION("Sync control when is active")
  {
    CommandQueueStub ctlCmds;

    REQUIRE_CALL(ts, syncControl(trompeloeil::_));
    ts.activate(true);
    ts.sync(ctlCmds);

    FORBID_CALL(ts, syncControl(trompeloeil::_));
    ts.activate(false);
    ts.sync(ctlCmds);
  }
}
} // namespace Control
} // namespace Tests
