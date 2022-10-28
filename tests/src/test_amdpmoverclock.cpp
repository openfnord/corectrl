// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/controls/amd/pm/advanced/overclock/pmoverclock.h"

namespace Tests {
namespace AMD {
namespace PMOverclock {

TEST_CASE("AMD PMOverclock tests", "[GPU][AMD][PM][PMAdvanced][PMOverclock]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has PMOverclock ID")
  {
    ::AMD::PMOverclock ts(std::move(controlMocks));
    REQUIRE(ts.ID() == ::AMD::PMOverclock::ItemID);
  }

  SECTION("Is active by default")
  {
    ::AMD::PMOverclock ts(std::move(controlMocks));
    REQUIRE(ts.active());
  }
}
} // namespace PMOverclock
} // namespace AMD
} // namespace Tests
