// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/controls/amd/pm/advanced/pmadvanced.h"

namespace Tests {
namespace AMD {
namespace PMAdvanced {

TEST_CASE("AMD PMAdvanced tests", "[GPU][AMD][PM][PMAdvanced]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has PMAdvanced ID")
  {
    ::AMD::PMAdvanced ts(std::move(controlMocks));
    REQUIRE(ts.ID() == ::AMD::PMAdvanced::ItemID);
  }

  SECTION("Is not active by default")
  {
    ::AMD::PMAdvanced ts(std::move(controlMocks));
    REQUIRE_FALSE(ts.active());
  }
}
} // namespace PMAdvanced
} // namespace AMD
} // namespace Tests
