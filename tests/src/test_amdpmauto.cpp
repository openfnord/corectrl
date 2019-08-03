//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
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
