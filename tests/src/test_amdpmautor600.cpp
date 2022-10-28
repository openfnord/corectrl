// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
