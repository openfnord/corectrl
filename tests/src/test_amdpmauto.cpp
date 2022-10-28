// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "core/components/controls/amd/pm/auto/pmauto.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMAuto {

class PMAutoTestAdapter : public ::AMD::PMAuto
{
 public:
  MAKE_MOCK1(cleanControl, void(ICommandQueue &), override);
  MAKE_MOCK1(syncControl, void(ICommandQueue &), override);
};

TEST_CASE("AMD PMAuto tests", "[GPU][AMD][PM][PMAuto]")
{
  PMAutoTestAdapter ts;

  SECTION("Has PMAuto ID")
  {
    REQUIRE(ts.ID() == ::AMD::PMAuto::ItemID);
  }

  SECTION("Is active by default")
  {
    REQUIRE(ts.active());
  }

  SECTION("Does not generate pre-init control commands")
  {
    CommandQueueStub cmds;
    ts.preInit(cmds);
    REQUIRE(cmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    CommandQueueStub cmds;
    ts.postInit(cmds);
    REQUIRE(cmds.commands().empty());
  }
}
} // namespace PMAuto
} // namespace AMD
} // namespace Tests
