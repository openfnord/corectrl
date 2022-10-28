// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/controls/amd/fan/fanmode.h"

namespace Tests {
namespace AMD {
namespace FanMode {

TEST_CASE("AMD FanMode tests", "[GPU][AMD][Fan][FanMode]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has FanMode ID")
  {
    ::AMD::FanMode ts(std::move(controlMocks));
    REQUIRE(ts.ID() == ::AMD::FanMode::ItemID);
  }

  SECTION("Is active by default")
  {
    ::AMD::FanMode ts(std::move(controlMocks));
    REQUIRE(ts.active());
  }
}
} // namespace FanMode
} // namespace AMD
} // namespace Tests
