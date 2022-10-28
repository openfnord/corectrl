// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpufreq.h"

#include "../gpusensorprovider.h"
#include "../graphitemprofilepart.h"
#include "../graphitemxmlparser.h"
#include "../sensor.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/devfsdatasource.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/info/vendor.h"
#include "core/iprofilepart.h"
#include "core/iprofilepartxmlparser.h"
#include "core/profilepartprovider.h"
#include "core/profilepartxmlparserprovider.h"
#include "units/units.h"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace AMD {
namespace GPUFreq {

class Provider final : public IGPUSensorProvider::IProvider
{
 public:
  std::vector<std::unique_ptr<ISensor>>
  provideGPUSensors(IGPUInfo const &gpuInfo, ISWInfo const &swInfo) const override
  {
    std::vector<std::unique_ptr<ISensor>> sensors;

    if (gpuInfo.vendor() == Vendor::AMD) {
      auto driver = gpuInfo.info(IGPUInfo::Keys::driver);
      auto kernel = Utils::String::parseVersion(
          swInfo.info(ISWInfo::Keys::kernelVersion));

      if (driver == "amdgpu" && kernel >= std::make_tuple(4, 12, 0)) {

#if defined(AMDGPU_INFO_SENSOR_GFX_SCLK)

        std::optional<
            std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
            range;

        // get range from dpm mclk states (4.6+)
        auto ppDPMMclkData =
            Utils::File::readFileLines(gpuInfo.path().sys / "pp_dpm_sclk");
        auto gpuStates = Utils::AMD::parseDPMStates(ppDPMMclkData);
        if (gpuStates.has_value() && !gpuStates->empty())
          range = {gpuStates->front().second, gpuStates->back().second};

        // enlarge range if needed using mem clk range
        auto ppDPMSclkData =
            Utils::File::readFileLines(gpuInfo.path().sys / "pp_dpm_mclk");
        auto memStates = Utils::AMD::parseDPMStates(ppDPMSclkData);
        if (memStates.has_value() && !memStates->empty())
          range = {std::min(memStates->front().second, range->first),
                   std::max(memStates->back().second, range->second)};

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        dataSources.emplace_back(std::make_unique<DevFSDataSource<unsigned int>>(
            gpuInfo.path().dev, [](int fd) {
              unsigned int value;
              bool success = Utils::AMD::readAMDGPUInfoSensor(
                  fd, &value, AMDGPU_INFO_SENSOR_GFX_SCLK);
              return success ? value : 0;
            }));

        sensors.emplace_back(
            std::make_unique<Sensor<units::frequency::megahertz_t, unsigned int>>(
                AMD::GPUFreq::ItemID, std::move(dataSources), std::move(range)));
#endif
      }
      else if (driver == "radeon" && kernel >= std::make_tuple(4, 1, 0)) {

#if defined(RADEON_INFO_CURRENT_GPU_SCLK)

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        dataSources.emplace_back(std::make_unique<DevFSDataSource<unsigned int>>(
            gpuInfo.path().dev, [](int fd) {
              unsigned int value;
              bool success = Utils::AMD::readRadeonInfoSensor(
                  fd, &value, RADEON_INFO_CURRENT_GPU_SCLK);
              return success ? value : 0;
            }));

        sensors.emplace_back(
            std::make_unique<Sensor<units::frequency::megahertz_t, unsigned int>>(
                AMD::GPUFreq::ItemID, std::move(dataSources)));
#endif
      }
    }

    return sensors;
  }
};

static bool register_()
{
  GPUSensorProvider::registerProvider(std::make_unique<AMD::GPUFreq::Provider>());

  ProfilePartProvider::registerProvider(AMD::GPUFreq::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::GPUFreq::ItemID,
                                                  "fuchsia");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::GPUFreq::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::GPUFreq::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace GPUFreq
} // namespace AMD
