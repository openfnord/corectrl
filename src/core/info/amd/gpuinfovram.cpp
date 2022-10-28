// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuinfovram.h"

#include "../common/gpuinfoueventdatasource.h"
#include "../common/swinfokerneldatasource.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/devfsdatasource.h"
#include "core/idatasource.h"
#include "core/info/infoproviderregistry.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <tuple>
#include <utility>

class AMDGPUInfoVRamDataSource
: public IDataSource<units::data::megabyte_t, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "";
  }

  bool read(units::data::megabyte_t &data,
            std::filesystem::path const &path) override
  {
    auto vramDataSource = DevFSDataSource<units::data::megabyte_t>(
        path, [](int fd) {
          units::data::megabyte_t size;
          Utils::AMD::readAMDGPUVRamSize(fd, &size);
          return size;
        });

    if (vramDataSource.read(data))
      return true;

    LOG(WARNING) << fmt::format("Cannot retrieve device memory size from {}",
                                path.c_str());
    return false;
  }
};

class RadeonGPUInfoVRamDataSource
: public IDataSource<units::data::megabyte_t, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "";
  }

  bool read(units::data::megabyte_t &data,
            std::filesystem::path const &path) override
  {
    auto vramDataSource = DevFSDataSource<units::data::megabyte_t>(
        path, [](int fd) {
          units::data::megabyte_t size;
          Utils::AMD::readRadeonVRamSize(fd, &size);
          return size;
        });

    if (vramDataSource.read(data))
      return true;

    LOG(WARNING) << fmt::format("Cannot retrieve device memory size from {}",
                                path.c_str());
    return false;
  }
};

GPUInfoVRam::GPUInfoVRam(
    std::unique_ptr<IDataSource<std::string>> &&kernelVersionDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>, std::filesystem::path const>>
        &&driverDataSource,
    std::unique_ptr<IDataSource<units::data::megabyte_t, std::filesystem::path const>>
        &&radeonDataSource,
    std::unique_ptr<IDataSource<units::data::megabyte_t, std::filesystem::path const>>
        &&amdgpuDataSource) noexcept
: kernelVersionDataSource_(std::move(kernelVersionDataSource))
, driverDataSource_(std::move(driverDataSource))
, radeonDataSource_(std::move(radeonDataSource))
, amdgpuDataSource_(std::move(amdgpuDataSource))
{
}

std::vector<std::pair<std::string, std::string>>
GPUInfoVRam::provideInfo(Vendor, int, IGPUInfo::Path const &path,
                         IHWIDTranslator const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  auto kernel = readKernelVersion();
  auto driver = readDriver(path.sys);

  if ((driver == "radeon" && kernel >= std::make_tuple(2, 6, 31)) ||
      (driver == "amdgpu" && kernel >= std::make_tuple(4, 10, 0))) {

    units::data::megabyte_t memory;
    bool success = false;

    if (driver == "radeon")
      success = radeonDataSource_->read(memory, path.dev);
    else if (driver == "amdgpu")
      success = amdgpuDataSource_->read(memory, path.dev);
    else
      LOG(WARNING) << "Cannot retrieve vram size: unsupported driver";

    if (success)
      info.emplace_back(IGPUInfo::Keys::memory,
                        fmt::format("{} {}", memory.template to<unsigned int>(),
                                    memory.abbreviation()));
  }

  return info;
}

std::tuple<int, int, int> GPUInfoVRam::readKernelVersion() const
{
  std::string data;
  if (kernelVersionDataSource_->read(data)) {
    auto version =
        Utils::String::parseKernelProcVersion(data).value_or("0.0.0");
    return Utils::String::parseVersion(version);
  }

  return std::make_tuple(0, 0, 0);
}

std::string GPUInfoVRam::readDriver(std::filesystem::path const &path) const
{
  std::string driver;
  std::vector<std::string> data;

  if (driverDataSource_->read(data, path)) {
    static constexpr std::string_view driverStr{"DRIVER"};

    for (auto &line : data) {
      if (line.find(driverStr) == 0) {
        driver = line.substr(driverStr.length() + 1, std::string::npos);
        break;
      }
    }

    if (driver.empty())
      LOG(ERROR) << "Cannot retrieve driver";
  }

  return driver;
}

std::vector<std::string> GPUInfoVRam::provideCapabilities(Vendor, int,
                                                          IGPUInfo::Path const &)
{
  return {};
}

bool GPUInfoVRam::registered_ = InfoProviderRegistry::add(
    std::make_unique<GPUInfoVRam>(std::make_unique<SWInfoKernelDataSource>(),
                                  std::make_unique<GPUInfoUeventDataSource>(),
                                  std::make_unique<RadeonGPUInfoVRamDataSource>(),
                                  std::make_unique<AMDGPUInfoVRamDataSource>()));
