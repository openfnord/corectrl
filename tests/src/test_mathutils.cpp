// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/mathutils.h"
#include "units/units.h"

namespace Tests {
namespace Utils {
namespace Math {

TEST_CASE("MathUtils tests", "[Utils][Math]")
{
  SECTION("linearNorm")
  {
    std::vector<int> values{1, 50, 100};
    std::pair<int, int> oldRange(1, 100);
    std::pair<int, int> newRange(1, 50);

    std::vector<int> normValues{1, 25, 50};
    ::Utils::Math::linearNorm(values, oldRange, newRange);
    REQUIRE(values == normValues);
  }

  SECTION("lerpX")
  {
    auto p1 = std::make_pair(0, 0);
    auto p2 = std::make_pair(10, 10);

    REQUIRE(::Utils::Math::lerpX(5, p1, p2) == 5);
  }

  SECTION("lerpY")
  {
    auto p1 = std::make_pair(0, 0);
    auto p2 = std::make_pair(10, 10);

    REQUIRE(::Utils::Math::lerpY(5, p1, p2) == 5);
  }
}
} // namespace Math
} // namespace Utils
} // namespace Tests
