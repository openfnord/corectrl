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
