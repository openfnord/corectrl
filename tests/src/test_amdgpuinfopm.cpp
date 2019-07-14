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
