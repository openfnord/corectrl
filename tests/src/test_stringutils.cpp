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
#include "trompeloeil.hpp"

#include "common/stringutils.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace Utils {
namespace String {

TEST_CASE("StringUtils tests", "[Utils][String]")
{
  SECTION("cleanPrefix")
  {
    SECTION("Returns a string without the prefix when the input string "
            "contains the prefix")
    {
      std::string prefix("prefix ");
      std::string data("prefix text");
      auto res = ::Utils::String::cleanPrefix(data, prefix);

      REQUIRE(res == "text");
    }

    SECTION("Returns the input string when the prefix is not a substring of "
            "the input string")
    {
      std::string prefix("prefix");
      std::string data("some data");
      auto res = ::Utils::String::cleanPrefix(data, prefix);

      REQUIRE(res == "some data");
    }

    SECTION("Returns an empty string when the input string equals the prefix")
    {
      std::string prefix("prefix");
      std::string data("prefix");
      auto res = ::Utils::String::cleanPrefix(data, prefix);

      REQUIRE(res == "");
    }

    SECTION("Returns the input string when it is shorter than the prefix")
    {
      std::string prefix("prefix");
      std::string data("pref");
      auto res = ::Utils::String::cleanPrefix(data, prefix);

      REQUIRE(res == "pref");
    }

    SECTION("Returns the input string when the prefix is empty")
    {
      std::string prefix("");
      std::string data("data");
      auto res = ::Utils::String::cleanPrefix(data, prefix);

      REQUIRE(res == "data");
    }
  }

  SECTION("parseVersion")
  {
    SECTION("Returns version when the input string is in 'mayor.minor.patch' "
            "format")
    {
      auto version = ::Utils::String::parseVersion("1.2.3");
      REQUIRE(version == std::make_tuple(1, 2, 3));
    }

    SECTION("Returns 0.0.0 version when the input string is not in "
            "'mayor.minor.patch' format")
    {
      auto version = ::Utils::String::parseVersion("not a version");
      REQUIRE(version == std::make_tuple(0, 0, 0));
    }
  }

  SECTION("split")
  {
    SECTION("Doesn't include the delimiter character in the results")
    {
      auto result = ::Utils::String::split(" ", ' ');
      REQUIRE(result.empty());
    }

    SECTION("Returns the original string when it doesn't contains the "
            "delimiter character")
    {
      auto result = ::Utils::String::split("one", ' ');
      REQUIRE(result.size() == 1);
      REQUIRE(result[0] == "one");
    }

    SECTION("Split a string into substrings")
    {
      auto result = ::Utils::String::split("one  two three ", ' ');
      REQUIRE(result.size() == 3);
      REQUIRE(result[0] == "one");
      REQUIRE(result[1] == "two");
      REQUIRE(result[2] == "three");
    }
  }
}
} // namespace String
} // namespace Utils
} // namespace Tests
