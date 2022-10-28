// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/commandqueuestub.h"
#include "common/stringdatasourcestub.h"
#include "core/components/controls/amd/pm/fixed/pmfixedlegacy.h"

namespace Tests {
namespace AMD {
namespace PMFixedLegacy {

class PMFixedLegacyTestAdapter : public ::AMD::PMFixedLegacy
{
 public:
  using ::AMD::PMFixedLegacy::PMFixedLegacy;

  using ::AMD::PMFixedLegacy::cleanControl;
  using ::AMD::PMFixedLegacy::mode;
  using ::AMD::PMFixedLegacy::modes;
  using ::AMD::PMFixedLegacy::syncControl;
};

TEST_CASE("AMD PMFixedLegacy tests", "[GPU][AMD][PM][PMFixed][Legacy]")
{
  std::string const powerMethodPath{"power_method"};
  std::string const powerProfilePath{"power_profile"};
  CommandQueueStub ctlCmds;

  SECTION("Has 'low' mode selected by default")
  {
    PMFixedLegacyTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.mode() == "low");
  }

  SECTION("Has [low, mid, high] modes")
  {
    std::vector<std::string> const powerProfiles{"low", "mid", "high"};
    PMFixedLegacyTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.modes() == powerProfiles);
  }

  SECTION("Does generate clean control commands")
  {
    PMFixedLegacyTestAdapter ts(
        std::make_unique<StringDataSourceStub>(powerMethodPath, "profile"),
        std::make_unique<StringDataSourceStub>(powerProfilePath, "low"));
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 2);

    auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
    REQUIRE(cmd0Path == powerMethodPath);
    REQUIRE(cmd0Value == "profile");

    auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
    REQUIRE(cmd1Path == powerProfilePath);
    REQUIRE(cmd1Value == "auto");
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMFixedLegacyTestAdapter ts(
        std::make_unique<StringDataSourceStub>(powerMethodPath, "profile"),
        std::make_unique<StringDataSourceStub>(powerProfilePath, "low"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("power_method is out of sync")
    {
      PMFixedLegacyTestAdapter ts(
          std::make_unique<StringDataSourceStub>(powerMethodPath, "_other_"),
          std::make_unique<StringDataSourceStub>(powerProfilePath, "low"));
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 2);

      auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
      REQUIRE(cmd0Path == powerMethodPath);
      REQUIRE(cmd0Value == "profile");

      auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
      REQUIRE(cmd1Path == powerProfilePath);
      REQUIRE(cmd1Value == "low");
    }

    SECTION("power_profile is out of sync")
    {
      PMFixedLegacyTestAdapter ts(
          std::make_unique<StringDataSourceStub>(powerMethodPath, "profile"),
          std::make_unique<StringDataSourceStub>(powerProfilePath, "_other_"));
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);

      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == powerProfilePath);
      REQUIRE(cmdValue == "low");
    }
  }

  SECTION("power_method and power_profile are out of sync")
  {
    PMFixedLegacyTestAdapter ts(
        std::make_unique<StringDataSourceStub>(powerMethodPath, "_other_"),
        std::make_unique<StringDataSourceStub>(powerProfilePath, "_other_"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 2);

    auto &[cmd0Path, cmd0Value] = ctlCmds.commands().front();
    REQUIRE(cmd0Path == powerMethodPath);
    REQUIRE(cmd0Value == "profile");

    auto &[cmd1Path, cmd1Value] = ctlCmds.commands().back();
    REQUIRE(cmd1Path == powerProfilePath);
    REQUIRE(cmd1Value == "low");
  }
}
} // namespace PMFixedLegacy
} // namespace AMD
} // namespace Tests
