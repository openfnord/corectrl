// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
