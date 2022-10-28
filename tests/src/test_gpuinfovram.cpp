// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/hwidtranslatorstub.h"
#include "core/idatasource.h"
#include "core/info/amd/gpuinfovram.h"
#include "units/units.h"
#include <memory>

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace GPUInfoVRam {

class KernelSWInfoDataSourceStub : public IDataSource<std::string>
{
 public:
  KernelSWInfoDataSourceStub(std::string data = "")
  : data_(std::move(data))
  {
  }

  std::string source() const override
  {
    return "/proc/version";
  }

  bool read(std::string &data) override
  {
    data = data_;
    return true;
  }

 private:
  std::string const data_;
};

class GPUInfoDataSourceStub
: public IDataSource<std::vector<std::string>, std::filesystem::path const>
{
 public:
  GPUInfoDataSourceStub(std::vector<std::string> data)
  : data_(std::move(data))
  {
  }

  std::string source() const override
  {
    return "";
  }

  bool read(std::vector<std::string> &data, std::filesystem::path const &) override
  {
    data = data_;
    return true;
  }

 private:
  std::vector<std::string> const data_;
};

class GPUInfoVRamDataSourceStub
: public IDataSource<units::data::megabyte_t, std::filesystem::path const>
{
 public:
  GPUInfoVRamDataSourceStub(units::data::megabyte_t data, bool success)
  : data_(std::move(data))
  , success_(success)
  {
  }

  std::string source() const override
  {
    return "";
  }

  bool read(units::data::megabyte_t &data, std::filesystem::path const &) override
  {
    data = data_;
    return success_;
  }

 private:
  units::data::megabyte_t const data_;
  bool const success_;
};

class HWIDTranslatorMock : public IHWIDTranslator
{
 public:
  MAKE_CONST_MOCK1(vendor, std::string(std::string const &));
  MAKE_CONST_MOCK2(device, std::string(std::string const &, std::string const &));
  MAKE_CONST_MOCK4(subdevice,
                   std::string(std::string const &, std::string const &,
                               std::string const &, std::string const &));
};

TEST_CASE("GPUInfoVRam tests", "[Info][GPUInfo][GPUInfoVRam]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorStub hwIDTranslator;

  SECTION("radeon driver")
  {
    auto driverDataSource = std::make_unique<GPUInfoDataSourceStub>(
        std::vector({std::string("DRIVER=radeon")}));
    auto amdgpuVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
        units::make_unit<units::data::megabyte_t>(16), true);

    SECTION("Valid vram data source")
    {
      auto radeonVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
          units::make_unit<units::data::megabyte_t>(8), true);

      SECTION("Provides vram size when kernel >= 2.6.31")
      {
        ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                             "Linux version 2.6.31_..."),
                         std::move(driverDataSource),
                         std::move(radeonVRamDataSource),
                         std::move(amdgpuVRamDataSource));

        auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
        auto memory = std::make_pair(std::string(::IGPUInfo::Keys::memory),
                                     std::string("8 MB"));

        REQUIRE_THAT(output, Catch::VectorContains(memory));
      }

      SECTION("Does not provide vram size when kernel < 2.6.31")
      {
        ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                             "Linux version 2.6.30_..."),
                         std::move(driverDataSource),
                         std::move(radeonVRamDataSource),
                         std::move(amdgpuVRamDataSource));

        auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

        REQUIRE(output.empty());
      }
    }

    SECTION(
        "Does not provide vram size when the vram data source cannot be read")
    {
      auto radeonVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
          units::make_unit<units::data::megabyte_t>(8), false);

      ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                           "Linux version 2.6.31_..."),
                       std::move(driverDataSource),
                       std::move(radeonVRamDataSource),
                       std::move(amdgpuVRamDataSource));

      auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

      REQUIRE(output.empty());
    }
  }

  SECTION("amdgpu driver")
  {
    auto driverDataSource = std::make_unique<GPUInfoDataSourceStub>(
        std::vector({std::string("DRIVER=amdgpu")}));
    auto radeonVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
        units::make_unit<units::data::megabyte_t>(8), true);

    SECTION("Valid vram data source")
    {
      auto amdgpuVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
          units::make_unit<units::data::megabyte_t>(16), true);

      SECTION("Provides vram size when kernel >= 4.10")
      {
        ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                             "Linux version 4.10.0_..."),
                         std::move(driverDataSource),
                         std::move(radeonVRamDataSource),
                         std::move(amdgpuVRamDataSource));

        auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
        auto memory = std::make_pair(std::string(::IGPUInfo::Keys::memory),
                                     std::string("16 MB"));

        REQUIRE_THAT(output, Catch::VectorContains(memory));
      }

      SECTION("Does not provide vram size when kernel < 4.10")
      {
        ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                             "Linux version 4.9.0_..."),
                         std::move(driverDataSource),
                         std::move(radeonVRamDataSource),
                         std::move(amdgpuVRamDataSource));

        auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

        REQUIRE(output.empty());
      }
    }

    SECTION(
        "Does not provide vram size when the vram data source cannot be read")
    {
      auto amdgpuVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
          units::make_unit<units::data::megabyte_t>(16), false);

      ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                           "Linux version 4.10.0_..."),
                       std::move(driverDataSource),
                       std::move(radeonVRamDataSource),
                       std::move(amdgpuVRamDataSource));

      auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

      REQUIRE(output.empty());
    }
  }

  SECTION("Does not provide vram size when using an unknown driver")
  {
    auto driverDataSource = std::make_unique<GPUInfoDataSourceStub>(
        std::vector({std::string("DRIVER=unknown")}));
    auto radeonVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
        units::make_unit<units::data::megabyte_t>(8), true);
    auto amdgpuVRamDataSource = std::make_unique<GPUInfoVRamDataSourceStub>(
        units::make_unit<units::data::megabyte_t>(16), true);

    ::GPUInfoVRam ts(std::make_unique<KernelSWInfoDataSourceStub>(
                         "Linux version 4.19.0_..."),
                     std::move(driverDataSource),
                     std::move(radeonVRamDataSource),
                     std::move(amdgpuVRamDataSource));

    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

    REQUIRE(output.empty());
  }
}
} // namespace GPUInfoVRam
} // namespace Tests
