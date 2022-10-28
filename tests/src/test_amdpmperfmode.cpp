// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/controls/amd/pm/pmperfmode.h"

namespace Tests {
namespace AMD {
namespace PMPerfMode {

TEST_CASE("AMD PMPerfMode tests", "[GPU][AMD][PM][PMPerfMode]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has PMPerfMode ID")
  {
    ::AMD::PMPerfMode ts(std::move(controlMocks));
    REQUIRE(ts.ID() == ::AMD::PMPerfMode::ItemID);
  }

  SECTION("Is active by default")
  {
    ::AMD::PMPerfMode ts(std::move(controlMocks));
    REQUIRE(ts.active());
  }
}
} // namespace PMPerfMode
} // namespace AMD
} // namespace Tests
