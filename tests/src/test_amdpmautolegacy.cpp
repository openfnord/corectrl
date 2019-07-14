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
#include "core/components/controls/amd/pm/auto/pmautolegacy.h"

namespace Tests {
namespace AMD {
namespace PMAutoLegacy {

class PMAutoLegacyTestAdapter : public ::AMD::PMAutoLegacy
{
 public:
  using ::AMD::PMAutoLegacy::PMAutoLegacy;

  using ::AMD::PMAutoLegacy::cleanControl;
  using ::AMD::PMAutoLegacy::syncControl;
};

TEST_CASE("AMD PMAutoLegacy tests", "[GPU][AMD][PM][PMAuto][Legacy]")
{
  std::string const powerMethodPath{"power_method"};
  std::string const powerProfilePath{"power_profile"};
  CommandQueueStub ctlCmds;

  SECTION("Does not generate clean control commands")
  {
    PMAutoLegacyTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                               std::make_unique<StringDataSourceStub>());
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMAutoLegacyTestAdapter ts(
        std::make_unique<StringDataSourceStub>(powerMethodPath, "profile"),
        std::make_unique<StringDataSourceStub>(powerProfilePath, "auto"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("power_method is out of sync")
    {
      PMAutoLegacyTestAdapter ts(
          std::make_unique<StringDataSourceStub>(powerMethodPath, "_other_"),
          std::make_unique<StringDataSourceStub>(powerProfilePath, "auto"));
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == powerMethodPath);
      REQUIRE(cmd0Value == "profile");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == powerProfilePath);
      REQUIRE(cmd1Value == "auto");
    }

    SECTION("power_profile is out of sync")
    {
      PMAutoLegacyTestAdapter ts(
          std::make_unique<StringDataSourceStub>(powerMethodPath, "profile"),
          std::make_unique<StringDataSourceStub>(powerProfilePath, "_other_"));
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == powerMethodPath);
      REQUIRE(cmd0Value == "profile");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == powerProfilePath);
      REQUIRE(cmd1Value == "auto");
    }
  }
}
} // namespace PMAutoLegacy
} // namespace AMD
} // namespace Tests
