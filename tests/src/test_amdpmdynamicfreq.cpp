// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
