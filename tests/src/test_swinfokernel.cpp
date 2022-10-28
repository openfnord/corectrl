// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/stringdatasourcestub.h"
#include "core/info/common/swinfokernel.h"

namespace Tests {
namespace SWInfoKernel {

TEST_CASE("SWInfoKernel tests", "[Info][SWInfo][SWInfoKernel]")
{
  SECTION("Provides kernel version when /proc/version has a known format")
  {
    std::string const infoData("Linux version 1.2.3_other_info ...");

    ::SWInfoKernel ts(
        std::make_unique<StringDataSourceStub>("/proc/version", infoData));
    auto output = ts.provideInfo();

    auto kernelVersion = std::make_pair(
        std::string(ISWInfo::Keys::kernelVersion), std::string("1.2.3"));
    REQUIRE_THAT(output, Catch::VectorContains(kernelVersion));
  }

  SECTION("Provides fake kernel version (0.0.0) when...")
  {
    SECTION("Cannot read /proc/version")
    {
      ::SWInfoKernel ts(
          std::make_unique<StringDataSourceStub>("/proc/version", "", false));
      auto output = ts.provideInfo();

      auto kernelVersion = std::make_pair(
          std::string(ISWInfo::Keys::kernelVersion), std::string("0.0.0"));
      REQUIRE_THAT(output, Catch::VectorContains(kernelVersion));
    }

    SECTION("/proc/version data has an unknown format")
    {
      std::string const infoData("Other format ...");

      ::SWInfoKernel ts(
          std::make_unique<StringDataSourceStub>("/proc/version", infoData));
      auto output = ts.provideInfo();

      auto kernelVersion = std::make_pair(
          std::string(ISWInfo::Keys::kernelVersion), std::string("0.0.0"));
      REQUIRE_THAT(output, Catch::VectorContains(kernelVersion));
    }
  }
}
} // namespace SWInfoKernel
} // namespace Tests
