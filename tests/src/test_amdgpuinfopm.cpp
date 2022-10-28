// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/stringpathdatasourcestub.h"
#include "core/info/amd/gpuinfopm.h"

namespace Tests {
namespace AMD {
namespace GPUInfoPM {

TEST_CASE("GPUInfoPM tests", "[AMD][Info][GPUInfo][GPUInfoPM]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  std::vector<std::shared_ptr<IDataSource<std::string, std::filesystem::path const>>>
      dataSources;

  SECTION("Provides Legacy capability")
  {
    dataSources.emplace_back(std::make_shared<StringPathDataSourceStub>(
        "power_method", "dynpm", true));
    dataSources.emplace_back(std::make_shared<StringPathDataSourceStub>(
        "power_method", "profile", true));
    ::AMD::GPUInfoPM ts(std::move(dataSources));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);
    REQUIRE(output.size() == 2);
    REQUIRE(output.front() == ::AMD::GPUInfoPM::Legacy);
    REQUIRE(output.back() == ::AMD::GPUInfoPM::Legacy);
  }

  SECTION("Provides Radeon capability")
  {

    dataSources.emplace_back(std::make_shared<StringPathDataSourceStub>(
        "power_method", "dpm", true));
    ::AMD::GPUInfoPM ts(std::move(dataSources));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);
    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPM::Radeon);
  }

  SECTION("Provides Amdgpu capability")
  {

    dataSources.emplace_back(std::make_shared<StringPathDataSourceStub>(
        "power_dpm_force_performance_level", "some_value", true));
    ::AMD::GPUInfoPM ts(std::move(dataSources));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);
    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPM::Amdgpu);
  }
}
} // namespace GPUInfoPM
} // namespace AMD
} // namespace Tests
