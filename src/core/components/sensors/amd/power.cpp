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
#include "power.h"

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

namespace fs = std::filesystem;

namespace AMD {
namespace Power {

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

      if (driver == "amdgpu" && kernel >= std::make_tuple(4, 12, 0)) {

#if defined(AMDGPU_INFO_SENSOR_GPU_AVG_POWER)

        std::optional<std::pair<units::power::watt_t, units::power::watt_t>> range;

        if (kernel >= std::make_tuple(4, 17, 0)) {
          auto path =
              Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
          if (path.has_value()) {

            auto power1CapMinPath = path.value() / "power1_cap_min";
            auto power1CapMaxPath = path.value() / "power1_cap_max";
            if (Utils::File::isSysFSEntryValid(power1CapMinPath) &&
                Utils::File::isSysFSEntryValid(power1CapMaxPath)) {

              auto power1CapMinLines =
                  Utils::File::readFileLines(power1CapMinPath);
              auto power1CapMaxLines =
                  Utils::File::readFileLines(power1CapMaxPath);
              unsigned long power1CapMinValue;
              unsigned long power1CapMaxValue;

              if (Utils::String::toNumber<unsigned long>(
                      power1CapMinValue, power1CapMinLines.front()) &&
                  Utils::String::toNumber<unsigned long>(
                      power1CapMaxValue, power1CapMaxLines.front())) {

                if (power1CapMinValue < power1CapMaxValue)
                  range = {units::power::microwatt_t(power1CapMinValue),
                           units::power::microwatt_t(power1CapMaxValue)};
              }
            }
          }
        }

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        dataSources.emplace_back(std::make_unique<DevFSDataSource<unsigned int>>(
            gpuInfo.path().dev, [](int fd) {
              unsigned int value;
              bool success = Utils::AMD::readAMDGPUInfoSensor(
                  fd, &value, AMDGPU_INFO_SENSOR_GPU_AVG_POWER);
              return success ? value : 0;
            }));

        return std::make_unique<Sensor<units::power::watt_t, unsigned int>>(
            AMD::Power::ItemID, std::move(dataSources), std::move(range));
#endif
      }
    }

    return nullptr;
  }
};

static bool register_()
{
  GPUSensorProvider::registerProvider(std::make_unique<AMD::Power::Provider>());

  ProfilePartProvider::registerProvider(AMD::Power::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::Power::ItemID, "gold");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::Power::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::Power::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace Power
} // namespace AMD
