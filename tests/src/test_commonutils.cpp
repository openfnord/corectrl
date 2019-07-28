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

#include "core/components/commonutils.h"
#include "units/units.h"

namespace Tests {
namespace Utils {
namespace Common {

TEST_CASE("CommonUtils tests", "[Utils][Common]")
{
  SECTION("normalizePoints")
  {
    std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
        points{std::make_pair(units::temperature::celsius_t(0),
                              units::concentration::percent_t(100)),
               std::make_pair(units::temperature::celsius_t(50),
                              units::concentration::percent_t(150)),
               std::make_pair(units::temperature::celsius_t(90),
                              units::concentration::percent_t(200))};

    auto tempRange = std::make_pair(units::temperature::celsius_t(0),
                                    units::temperature::celsius_t(50));

    ::Utils::Common::normalizePoints(points, tempRange);

    // temperature range
    REQUIRE_FALSE(std::any_of(points.cbegin(), points.cend(), [&](auto &point) {
      return point.first < tempRange.first || point.first > tempRange.second;
    }));

    // percentage range
    REQUIRE_FALSE(std::any_of(points.cbegin(), points.cend(), [&](auto &point) {
      return point.second < units::concentration::percent_t(0) ||
             point.second > units::concentration::percent_t(100);
    }));
  }
}
} // namespace Common
} // namespace Utils
} // namespace Tests
