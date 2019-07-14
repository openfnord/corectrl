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
#include "core/components/controls/amd/pm/fixed/pmfixedr600.h"

namespace Tests {
namespace AMD {
namespace PMFixedR600 {

class PMFixedR600TestAdapter : public ::AMD::PMFixedR600
{
 public:
  using ::AMD::PMFixedR600::PMFixedR600;

  using ::AMD::PMFixedR600::cleanControl;
  using ::AMD::PMFixedR600::mode;
  using ::AMD::PMFixedR600::modes;
  using ::AMD::PMFixedR600::syncControl;
};

TEST_CASE("AMD PMFixedR600 tests", "[GPU][AMD][PM][PMFixed][R600]")
{
  std::string const perfLevelPath{"power_dpm_force_performance_level"};
  CommandQueueStub ctlCmds;

  SECTION("Has 'low' mode selected by default")
  {
    PMFixedR600TestAdapter ts(std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.mode() == "low");
  }

  SECTION("Has [low, high] modes")
  {
    std::vector<std::string> const perfLevels{"low", "high"};
    PMFixedR600TestAdapter ts(std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.modes() == perfLevels);
  }

  SECTION("Does generate clean control commands")
  {
    PMFixedR600TestAdapter ts(
        std::make_unique<StringDataSourceStub>(perfLevelPath, "low"));
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);

    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == perfLevelPath);
    REQUIRE(value == "auto");
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMFixedR600TestAdapter ts(
        std::make_unique<StringDataSourceStub>(perfLevelPath, "low"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    PMFixedR600TestAdapter ts(
        std::make_unique<StringDataSourceStub>(perfLevelPath, "_other_"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);

    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == perfLevelPath);
    REQUIRE(value == "low");
  }
}

} // namespace PMFixedR600
} // namespace AMD
} // namespace Tests
