// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/commandqueuestub.h"
#include "core/components/controls/noop.h"

namespace Tests {
namespace Noop {

class NoopTestAdapter : public ::Noop
{
 public:
  using ::Noop::Noop;

  using ::Noop::cleanControl;
  using ::Noop::syncControl;
};

TEST_CASE("Noop control tests", "[Noop]")
{
  CommandQueueStub ctlCmds;

  SECTION("Has Noop ID")
  {
    NoopTestAdapter ts;

    REQUIRE(ts.ID() == ::Noop::ItemID);
  }

  SECTION("Is active by default")
  {
    NoopTestAdapter ts;

    REQUIRE(ts.active());
  }

  SECTION("Does not generate pre-init control commands")
  {
    NoopTestAdapter ts;
    ts.preInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    NoopTestAdapter ts;
    ts.postInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate clean control commands")
  {
    NoopTestAdapter ts;
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands")
  {
    NoopTestAdapter ts;
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }
}
} // namespace Noop
} // namespace Tests
