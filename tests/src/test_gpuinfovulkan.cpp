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
