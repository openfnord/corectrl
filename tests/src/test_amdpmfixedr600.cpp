// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
