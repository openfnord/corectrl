// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "memusage.h"

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
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace AMD {
namespace MemUsage {

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

      std::optional<std::pair<units::data::megabyte_t, units::data::megabyte_t>> range;

      auto memInfo = gpuInfo.info(IGPUInfo::Keys::memory);
      if (!memInfo.empty()) {
        unsigned int memorySize;
        if (Utils::String::toNumber<unsigned int>(memorySize, memInfo))
          range = {units::data::megabyte_t(0),
                   units::data::megabyte_t(memorySize)};
      }

      if (driver == "amdgpu" && kernel >= std::make_tuple(4, 2, 0)) {

#if defined(AMDGPU_INFO_VRAM_USAGE)

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        dataSources.emplace_back(std::make_unique<DevFSDataSource<unsigned int>>(
            gpuInfo.path().dev, [](int fd) {
              std::uint64_t value;
              bool success = Utils::AMD::readAMDGPUInfo(fd, &value,
                                                        AMDGPU_INFO_VRAM_USAGE);
              return success ? value / (1024 * 1024) : 0;
            }));

        sensors.emplace_back(
            std::make_unique<Sensor<units::data::megabyte_t, unsigned int>>(
                AMD::MemUsage::ItemID, std::move(dataSources), std::move(range)));
#endif
      }
      else if (driver == "radeon" && kernel >= std::make_tuple(3, 15, 0)) {

#if defined(RADEON_INFO_VRAM_USAGE)

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        dataSources.emplace_back(std::make_unique<DevFSDataSource<unsigned int>>(
            gpuInfo.path().dev, [](int fd) {
              std::uint64_t value;
              bool success = Utils::AMD::readRadeonInfoSensor(
                  fd, &value, RADEON_INFO_VRAM_USAGE);
              return success ? value / (1024 * 1024) : 0;
            }));

        sensors.emplace_back(
            std::make_unique<Sensor<units::data::megabyte_t, unsigned int>>(
                AMD::MemUsage::ItemID, std::move(dataSources), std::move(range)));
#endif
      }
    }

    return sensors;
  }
};

static bool register_()
{
  GPUSensorProvider::registerProvider(
      std::make_unique<AMD::MemUsage::Provider>());

  ProfilePartProvider::registerProvider(AMD::MemUsage::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::MemUsage::ItemID,
                                                  "forestgreen");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::MemUsage::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::MemUsage::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace MemUsage
} // namespace AMD
