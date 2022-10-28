// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/idatasource.h"
#include "core/info/amd/gpuinfopmoverdrive.h"

namespace Tests {
namespace AMD {
namespace GPUInfoPMOverdrive {

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

TEST_CASE("GPUInfoPMOverdrive tests",
          "[AMD][Info][GPUInfo][GPUInfoPMOverdrive]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");

  SECTION("Provides voltage curve capability")
  {
    std::vector<std::string> ppOdClkVoltageData{"OD_VDDC_CURVE:"};

    ::AMD::GPUInfoPMOverdrive ts(std::make_unique<VectorStringPathDataSourceStub>(
        "pp_od_clk_voltage", std::move(ppOdClkVoltageData)));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);

    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPMOverdrive::VoltCurve);
  }

  SECTION("Provides clock + voltage capability")
  {
    // clang-format off
    std::vector<std::string> ppOdClkVoltageData{"OD_SCLK:",
                                                "0: 300MHz 800mV"};
    // clang-format on

    ::AMD::GPUInfoPMOverdrive ts(std::make_unique<VectorStringPathDataSourceStub>(
        "pp_od_clk_voltage", std::move(ppOdClkVoltageData)));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);

    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPMOverdrive::ClkVolt);
  }

  SECTION("Provides clock capability")
  {
    // clang-format off
    std::vector<std::string> ppOdClkVoltageData{"OD_SCLK:",
                                                "0: 300MHz"};
    // clang-format on

    ::AMD::GPUInfoPMOverdrive ts(std::make_unique<VectorStringPathDataSourceStub>(
        "pp_od_clk_voltage", std::move(ppOdClkVoltageData)));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);

    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPMOverdrive::Clk);
  }

  SECTION("Provides voltage offset capability")
  {
    // clang-format off
    std::vector<std::string> ppOdClkVoltageData{"OD_VDDGFX_OFFSET:",
                                                "0mV"};
    // clang-format on

    ::AMD::GPUInfoPMOverdrive ts(std::make_unique<VectorStringPathDataSourceStub>(
        "pp_od_clk_voltage", std::move(ppOdClkVoltageData)));

    auto output = ts.provideCapabilities(vendor, gpuIndex, path);

    REQUIRE(output.size() == 1);
    REQUIRE(output.front() == ::AMD::GPUInfoPMOverdrive::VoltOffset);
  }
}
} // namespace GPUInfoPMOverdrive
} // namespace AMD
} // namespace Tests
