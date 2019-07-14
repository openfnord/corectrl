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

#include "core/idatasource.h"
#include "core/info/amd/gpuinfopmodcv.h"

namespace Tests {
namespace AMD {
namespace GPUInfoPMODCV {

class VectorStringPathDataSourceStub
: public IDataSource<std::vector<std::string>, std::filesystem::path const>
{
 public:
  VectorStringPathDataSourceStub(std::string_view source = "",
                                 std::vector<std::string> data = {""},
                                 bool success = true) noexcept
  : source_(source)
  , data_(data)
  , success_(success)
  {
  }

  std::string source() const override
  {
    return source_;
  }

  bool read(std::vector<std::string> &data, std::filesystem::path const &) override
  {
    data = data_;
    return success_;
  }

  std::string const source_;
  std::vector<std::string> const data_;
  bool success_;
};

TEST_CASE("GPUInfoPMODCV tests", "[AMD][Info][GPUInfo][GPUInfoPMODCV]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");

  SECTION("Provides Curve capability")
  {
    std::vector<std::string> ppOdClkVoltageData{"...\
OD_VDDC_CURVE:\
...\
VDDC_CURVE_SCLK..."};

    ::AMD::GPUInfoPMODCV ts(std::make_unique<VectorStringPathDataSourceStub>(
        "pp_od_clk_voltage", std::move(ppOdClkVoltageData)));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);

    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPMODCV::Curve);
  }

  SECTION("Provides Fixed capability")
  {
    std::vector<std::string> ppOdClkVoltageData{"...\
OD_SCLK:\
...\
OD_RANGE:\
..."};

    ::AMD::GPUInfoPMODCV ts(std::make_unique<VectorStringPathDataSourceStub>(
        "pp_od_clk_voltage", std::move(ppOdClkVoltageData)));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);

    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPMODCV::Fixed);
  }
}
} // namespace GPUInfoPMODCV
} // namespace AMD
} // namespace Tests
