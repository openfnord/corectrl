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
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/handlers/ppdpmhandler.h"

namespace Tests {
namespace AMD {
namespace PpDpmHandler {

TEST_CASE("AMD PpDpmHandler tests",
          "[GPU][AMD][PM][DataSourceHandler][PpDpmHandler]")
{
  CommandQueueStub ctlCmds;

  std::vector<unsigned int> defaultActiveStates{0, 1};
  std::vector<std::string> ppDpmData{"0: 300Mhz *", "1: 2000Mhz"};

  ::AMD::PpDpmHandler ts(
      std::make_unique<VectorStringDataSourceStub>("pp_dpm_sclk", ppDpmData));

  SECTION("Initializes states from data source on construction")
  {

    auto states = ts.states();
    REQUIRE(states.size() == 2);
    REQUIRE(states.front() ==
            std::make_pair(0u, units::frequency::megahertz_t(300)));
    REQUIRE(states.back() ==
            std::make_pair(1u, units::frequency::megahertz_t(2000)));

    SECTION("Has all states active by default")
    {
      REQUIRE(ts.active() == defaultActiveStates);
    }

    SECTION("Activate...")
    {
      SECTION("Works with known states")
      {
        ts.activate({1});
        auto &activeStates = ts.active();
        REQUIRE(activeStates.size() == 1);
        REQUIRE(activeStates.front() == 1);
      }

      SECTION("Ignores unknown states")
      {
        ts.activate({2, 3});
        REQUIRE(ts.active() == defaultActiveStates);
      }

      SECTION("Cannot deactivate all states")
      {
        ts.activate({});
        REQUIRE(ts.active() == defaultActiveStates);
      }
    }
  }

  SECTION("Saving and restoring data source state are not supported")
  {
    ts.saveState();
    ts.restoreState(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Queue reset control commands unconditionally")
  {
    ts.reset(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);
    auto &[cmdPath, cmdValue] = commands.front();
    REQUIRE(cmdPath == "pp_dpm_sclk");
    REQUIRE(cmdValue == "0 1");
  }

  SECTION("Queue apply control commands unconditionally")
  {
    ts.apply(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);
    auto &[cmdPath, cmdValue] = commands.front();
    REQUIRE(cmdPath == "pp_dpm_sclk");
    REQUIRE(cmdValue == "0 1");
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    // skip initial sync commands
    ts.sync(ctlCmds);
    ctlCmds.clear();

    ts.sync(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("Active states were changed")
    {
      ts.activate({0});
      ts.sync(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);
      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pp_dpm_sclk");
      REQUIRE(cmdValue == "0");
    }

    SECTION("Current index is not an active index")
    {
      ts.activate({1});
      ts.sync(ctlCmds);
      ctlCmds.clear();

      ts.sync(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);
      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pp_dpm_sclk");
      REQUIRE(cmdValue == "1");
    }
  }
}

} // namespace PpDpmHandler
} // namespace AMD
} // namespace Tests
