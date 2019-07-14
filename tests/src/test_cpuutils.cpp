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

#include "core/components/cpuutils.h"

namespace Tests {
namespace Utils {
namespace CPU {

TEST_CASE("CPU utils tests", "[Utils][CPU]")
{
  SECTION("parseProcCpuInfo")
  {
    // clang-format off
    std::vector<std::string> input{"processor	: 0",
                                   "target	: data"};
    // clang-format on

    SECTION("Returns target data")
    {
      auto data = ::Utils::CPU::parseProcCpuInfo(input, 0, "target");
      REQUIRE(data.has_value());
      REQUIRE(*data == "data");
    }

    SECTION("Returns nothing when input doesn't have the requested processor")
    {
      auto data = ::Utils::CPU::parseProcCpuInfo(input, 10, "target");
      REQUIRE_FALSE(data.has_value());
    }

    SECTION("Returns nothing when input doesn't have the requested target on "
            "the processor")
    {
      auto data = ::Utils::CPU::parseProcCpuInfo(input, 0, "wrong_target");
      REQUIRE_FALSE(data.has_value());
    }
  }
}

} // namespace CPU
} // namespace Utils
} // namespace Tests
