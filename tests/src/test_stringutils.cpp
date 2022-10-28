// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

  SECTION("parseKernelProcVersion")
  {
    SECTION("Provides kernel version when data is using...")
    {
      SECTION("Semver format")
      {
        std::string const data("Linux version 1.2.3_other_info ...");
        auto output = ::Utils::String::parseKernelProcVersion(data);
        REQUIRE(output.has_value());
        REQUIRE(*output == "1.2.3");
      }

      SECTION("Invalid semver format...")
      {
        SECTION("Missing patch version (see #254)")
        {
          std::string const data("Linux version 1.2_other_info ...");
          auto output = ::Utils::String::parseKernelProcVersion(data);
          REQUIRE(output.has_value());
          REQUIRE(*output == "1.2.0");
        }

        SECTION("With additional version numbers after patch version")
        {
          std::string const data("Linux version 1.2.3.4_other_info ...");
          auto output = ::Utils::String::parseKernelProcVersion(data);
          REQUIRE(output.has_value());
          REQUIRE(*output == "1.2.3");
        }
      }
    }

    SECTION("Returns no value when the string follow an unknown format")
    {
      std::string const data("Other format");
      auto output = ::Utils::String::parseKernelProcVersion(data);
      REQUIRE_FALSE(output.has_value());
    }
  }
}
} // namespace String
} // namespace Utils
} // namespace Tests
