// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
