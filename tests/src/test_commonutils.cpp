// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/commonutils.h"
#include "units/units.h"

namespace Tests {
namespace Utils {
namespace Common {

TEST_CASE("CommonUtils tests", "[Utils][Common]")
{
  SECTION("normalizePoints...")
  {
    SECTION("clamp points into range")
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

    SECTION("clamp inner points y-axis coordinates into [prev.y, next.y] range")
    {
      std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
          points{std::make_pair(units::temperature::celsius_t(0),
                                units::concentration::percent_t(20)),
                 std::make_pair(units::temperature::celsius_t(30),
                                units::concentration::percent_t(10)),
                 std::make_pair(units::temperature::celsius_t(60),
                                units::concentration::percent_t(30)),
                 std::make_pair(units::temperature::celsius_t(100),
                                units::concentration::percent_t(25))};

      std::vector<std::pair<units::temperature::celsius_t, units::concentration::percent_t>>
          normalizedPoints{std::make_pair(units::temperature::celsius_t(0),
                                          units::concentration::percent_t(20)),
                           std::make_pair(units::temperature::celsius_t(30),
                                          units::concentration::percent_t(20)),
                           std::make_pair(units::temperature::celsius_t(60),
                                          units::concentration::percent_t(30)),
                           std::make_pair(units::temperature::celsius_t(100),
                                          units::concentration::percent_t(30))};

      auto tempRange = std::make_pair(units::temperature::celsius_t(0),
                                      units::temperature::celsius_t(100));

      ::Utils::Common::normalizePoints(points, tempRange);

      REQUIRE_THAT(points, Catch::Matchers::Equals(normalizedPoints));
    }
  }
}
} // namespace Common
} // namespace Utils
} // namespace Tests
