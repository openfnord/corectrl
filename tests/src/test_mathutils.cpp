//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
