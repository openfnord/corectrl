//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
