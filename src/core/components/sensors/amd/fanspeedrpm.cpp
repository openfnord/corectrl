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
#include "fanspeedrpm.h"

#include "../gpusensorprovider.h"
#include "../graphitemprofilepart.h"
#include "../graphitemxmlparser.h"
#include "../sensor.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/info/vendor.h"
#include "core/iprofilepart.h"
#include "core/iprofilepartxmlparser.h"
#include "core/profilepartprovider.h"
#include "core/profilepartxmlparserprovider.h"
#include "core/sysfsdatasource.h"
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
namespace FanSpeedRPM {

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

      if (driver == "amdgpu" && kernel >= std::make_tuple(4, 10, 0)) {

        auto path =
            Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
        if (path.has_value()) {

          auto fanInput = path.value() / "fan1_input";
          auto pwm = path.value() / "pwm1";
          if (Utils::File::isSysFSEntryValid(fanInput) &&
              Utils::File::isSysFSEntryValid(pwm)) {

            std::optional<
                std::pair<units::angular_velocity::revolutions_per_minute_t,
                          units::angular_velocity::revolutions_per_minute_t>>
                range({units::angular_velocity::revolutions_per_minute_t(0),
                       units::angular_velocity::revolutions_per_minute_t(2200)});

            if (kernel >= std::make_tuple(4, 20, 0)) {
              auto min = Utils::File::readFileLines(path.value() / "fan1_min");
              auto max = Utils::File::readFileLines(path.value() / "fan1_max");
              if (!min.empty() && !max.empty()) {
                unsigned int minValue;
                unsigned int maxValue;
                if (Utils::String::toNumber<unsigned int>(minValue, min.front()) &&
                    Utils::String::toNumber<unsigned int>(maxValue, max.front()))
                  if (min < max) {
                    range = {units::angular_velocity::revolutions_per_minute_t(
                                 minValue),
                             units::angular_velocity::revolutions_per_minute_t(
                                 maxValue)};
                  }
              }
            }

            unsigned int value;
            auto fanInputLines = Utils::File::readFileLines(pwm);
            auto fanInputValid = Utils::String::toNumber<unsigned int>(
                value, fanInputLines.front());
            auto pwmLines = Utils::File::readFileLines(pwm);
            auto pwmValid = Utils::String::toNumber<unsigned int>(
                value, pwmLines.front());

            if (fanInputValid && pwmValid) {

              std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
              dataSources.emplace_back(
                  std::make_unique<SysFSDataSource<unsigned int>>(
                      fanInput, [](std::string const &data, unsigned int &output) {
                        Utils::String::toNumber<unsigned int>(output, data);
                      }));
              dataSources.emplace_back(
                  std::make_unique<SysFSDataSource<unsigned int>>(
                      pwm, [](std::string const &data, unsigned int &output) {
                        Utils::String::toNumber<unsigned int>(output, data);
                      }));

              return std::make_unique<Sensor<
                  units::angular_velocity::revolutions_per_minute_t, unsigned int>>(
                  AMD::FanSpeedRPM::ItemID, std::move(dataSources),
                  std::move(range), [](std::vector<unsigned int> const &input) {
                    return input[1] > 0 ? input[0] : 0;
                  });
            }
            else {
              if (!fanInputValid) {
                LOG(WARNING) << fmt::format("Unknown data format on {}",
                                            fanInput.string());
                LOG(ERROR) << fanInputLines.front().c_str();
              }

              if (!pwmValid) {
                LOG(WARNING)
                    << fmt::format("Unknown data format on {}", pwm.string());
                LOG(ERROR) << pwmLines.front().c_str();
              }
            }
          }
        }
      }
    }

    return nullptr;
  }
};

static bool register_()
{
  GPUSensorProvider::registerProvider(
      std::make_unique<AMD::FanSpeedRPM::Provider>());

  ProfilePartProvider::registerProvider(AMD::FanSpeedRPM::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::FanSpeedRPM::ItemID,
                                                  "lightskyblue");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::FanSpeedRPM::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::FanSpeedRPM::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace FanSpeedRPM
} // namespace AMD
