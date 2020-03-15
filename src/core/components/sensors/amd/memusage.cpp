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
  std::unique_ptr<ISensor> provideGPUSensor(IGPUInfo const &gpuInfo,
                                            ISWInfo const &swInfo) const override
  {
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
              unsigned int value;
              bool success = Utils::AMD::readAMDGPUInfo(fd, &value,
                                                        AMDGPU_INFO_VRAM_USAGE);
              return success ? value / (1024 * 1024) : 0;
            }));

        return std::make_unique<Sensor<units::data::megabyte_t, unsigned int>>(
            AMD::MemUsage::ItemID, std::move(dataSources), std::move(range));
#endif
      }
      else if (driver == "radeon" && kernel >= std::make_tuple(3, 15, 0)) {

#if defined(RADEON_INFO_VRAM_USAGE)

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        dataSources.emplace_back(std::make_unique<DevFSDataSource<unsigned int>>(
            gpuInfo.path().dev, [](int fd) {
              unsigned int value;
              bool success = Utils::AMD::readRadeonInfoSensor(
                  fd, &value, RADEON_INFO_VRAM_USAGE);
              return success ? value / (1024 * 1024) : 0;
            }));

        return std::make_unique<Sensor<units::data::megabyte_t, unsigned int>>(
            AMD::MemUsage::ItemID, std::move(dataSources), std::move(range));
#endif
      }
    }

    return nullptr;
  }
};

static bool register_()
{
  GPUSensorProvider::registerProvider(
      std::make_unique<AMD::MemUsage::Provider>());

  ProfilePartProvider::registerProvider(AMD::MemUsage::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::MemUsage::ItemID,
                                                  "darkred");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::MemUsage::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::MemUsage::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace MemUsage
} // namespace AMD
