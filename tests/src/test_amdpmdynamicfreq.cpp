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
#include "core/components/controls/amd/pm/advanced/dynamicfreq/pmdynamicfreq.h"

namespace Tests {
namespace AMD {
namespace PMDynamicFreq {

class PMDynamicFreqTestAdapter : public ::AMD::PMDynamicFreq
{
 public:
  using ::AMD::PMDynamicFreq::PMDynamicFreq;

  using ::AMD::PMDynamicFreq::cleanControl;
  using ::AMD::PMDynamicFreq::syncControl;
};

TEST_CASE("AMD PMDynamicFreq tests",
          "[GPU][AMD][PM][PMAdvanced][PMDynamicFreq]")
{
  std::string const path{"power_dpm_force_performance_level"};
  CommandQueueStub ctlCmds;

  SECTION("Has PMDynamicFreq ID")
  {
    PMDynamicFreqTestAdapter ts(std::make_unique<StringDataSourceStub>());
    REQUIRE(ts.ID() == ::AMD::PMDynamicFreq::ItemID);
  }

  SECTION("Is not active by default")
  {
    PMDynamicFreqTestAdapter ts(std::make_unique<StringDataSourceStub>());
    REQUIRE_FALSE(ts.active());
  }

  SECTION("Does not generate pre-init control commands")
  {
    PMDynamicFreqTestAdapter ts(std::make_unique<StringDataSourceStub>());
    ts.preInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    PMDynamicFreqTestAdapter ts(std::make_unique<StringDataSourceStub>());
    ts.postInit(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate clean control commands")
  {
    PMDynamicFreqTestAdapter ts(std::make_unique<StringDataSourceStub>());
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMDynamicFreqTestAdapter ts(
        std::make_unique<StringDataSourceStub>(path, "auto"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    PMDynamicFreqTestAdapter ts(
        std::make_unique<StringDataSourceStub>(path, "_other_"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);
    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == path);
    REQUIRE(value == "auto");
  }
}
} // namespace PMDynamicFreq
} // namespace AMD
} // namespace Tests
