// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/commandqueuestub.h"
#include "common/uintdatasourcestub.h"
#include "core/components/controls/amd/fan/auto/fanauto.h"

namespace Tests {
namespace AMD {
namespace FanAuto {

class FanAutoTestAdapter : public ::AMD::FanAuto
{
 public:
  using ::AMD::FanAuto::FanAuto;

  using ::AMD::FanAuto::cleanControl;
  using ::AMD::FanAuto::syncControl;
};

TEST_CASE("AMD FanAuto tests", "[GPU][AMD][Fan][FanAuto]")
{
  CommandQueueStub ctlCmds;

  SECTION("Has FanAuto ID")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>());

    REQUIRE(ts.ID() == ::AMD::FanAuto::ItemID);
  }

  SECTION("Is active by default")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>());

    REQUIRE(ts.active());
  }

  SECTION("Does not generate pre-init control commands")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>());
    ts.preInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>());
    ts.postInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate clean control commands")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>());
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>("path", 2));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    FanAutoTestAdapter ts(std::make_unique<UIntDataSourceStub>("path", 123));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);
    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == "path");
    REQUIRE(value == "2"); // 2 == automatic
  }
}
} // namespace FanAuto
} // namespace AMD
} // namespace Tests
