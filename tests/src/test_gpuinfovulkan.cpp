// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/hwidtranslatorstub.h"
#include "common/stringdatasourcestub.h"
#include "core/info/common/gpuinfovulkan.h"

namespace Tests {
namespace GPUInfoVulkan {

TEST_CASE("GPUInfoVulkan tests", "[Info][GPUInfo][GPUInfoVulkan]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorStub hwIDTranslator;

  SECTION("Provides api version (v1)")
  {
    std::string const infoData(
        // clang-format off
"...\n\
VkPhysicalDeviceProperties:\n\
...\n\
	apiVersion     = 4194306\n\
..."); // clang-format on

    ::GPUInfoVulkan ts(
        std::make_unique<StringDataSourceStub>("vulkaninfo", infoData));

    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

    auto apiVersion = std::make_pair(
        std::string(::GPUInfoVulkan::Keys::apiVersion), std::string("4194306"));
    REQUIRE_THAT(output, Catch::VectorContains(apiVersion));
  }

  SECTION("Provides api version (v2)")
  {
    std::string const infoData(
        // clang-format off
"...\n\
VkPhysicalDeviceProperties:\n\
...\n\
	apiVersion     = 0x401046  (1.1.70)\n\
..."); // clang-format on

    ::GPUInfoVulkan ts(
        std::make_unique<StringDataSourceStub>("vulkaninfo", infoData));

    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

    auto apiVersion = std::make_pair(
        std::string(::GPUInfoVulkan::Keys::apiVersion), std::string("1.1.70"));
    REQUIRE_THAT(output, Catch::VectorContains(apiVersion));
  }
}
} // namespace GPUInfoVulkan
} // namespace Tests
