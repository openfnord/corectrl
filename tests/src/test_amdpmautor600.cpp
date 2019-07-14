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
#include "common/stringdatasourcestub.h"
#include "core/components/controls/amd/pm/auto/pmautor600.h"

namespace Tests {
namespace AMD {
namespace PMAutoR600 {

class PMAutoR600TestAdapter : public ::AMD::PMAutoR600
{
 public:
  using ::AMD::PMAutoR600::PMAutoR600;

  using ::AMD::PMAutoR600::cleanControl;
  using ::AMD::PMAutoR600::syncControl;
};

TEST_CASE("AMD PMAutoR600 tests", "[GPU][AMD][PM][PMAuto][R600]")
{
  std::string const perfLevelPath{"power_dpm_force_performance_level"};
  CommandQueueStub ctlCmds;

  SECTION("Does not generate clean control commands")
  {
    PMAutoR600TestAdapter ts(std::make_unique<StringDataSourceStub>());
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMAutoR600TestAdapter ts(
        std::make_unique<StringDataSourceStub>(perfLevelPath, "auto"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    PMAutoR600TestAdapter ts(
        std::make_unique<StringDataSourceStub>(perfLevelPath, "_other_"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);
    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == perfLevelPath);
    REQUIRE(value == "auto");
  }
}
} // namespace PMAutoR600
} // namespace AMD
} // namespace Tests
