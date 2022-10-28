// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

namespace AMD {
namespace FanSpeedRPM {

// NOTE FanSpeedRPM is the preferred fan speed sensor.
//
// It will be used on any system with proper fan1_input and pwm1 support
// (most hardware using the amdgpu driver).
//
// pwm1 sensor is used to report 0 fan speed when the fans are stopped, as
// fan1_input could report bad values in this case.
// Related bug report: https://gitlab.freedesktop.org/drm/amd/-/issues/335
//
// See #184.

class Provider final : public IGPUSensorProvider::IProvider
{
 public:
  std::vector<std::unique_ptr<ISensor>>
  provideGPUSensors(IGPUInfo const &gpuInfo, ISWInfo const &swInfo) const override
  {
    std::vector<std::unique_ptr<ISensor>> sensors;

    if (gpuInfo.vendor() == Vendor::AMD) {

      auto path =
          Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
      if (path.has_value()) {

        std::vector<std::string> fileLines;
        auto fanInput = path.value() / "fan1_input";
        if (Utils::File::isSysFSEntryValid(fanInput)) {

          unsigned int value;
          fileLines = Utils::File::readFileLines(fanInput);
          if (Utils::String::toNumber<unsigned int>(value, fileLines.front())) {

            auto pwm = path.value() / "pwm1";
            if (Utils::File::isSysFSEntryValid(pwm)) {

              fileLines = Utils::File::readFileLines(pwm);
              if (Utils::String::toNumber<unsigned int>(value,
                                                        fileLines.front())) {
                // fallback sensor range
                std::optional<
                    std::pair<units::angular_velocity::revolutions_per_minute_t,
                              units::angular_velocity::revolutions_per_minute_t>>
                    range({units::angular_velocity::revolutions_per_minute_t(0),
                           units::angular_velocity::revolutions_per_minute_t(
                               2200)});

                // read the actual sensor range if supported
                auto kernel = Utils::String::parseVersion(
                    swInfo.info(ISWInfo::Keys::kernelVersion));
                if (kernel >= std::make_tuple(4, 20, 0)) {
                  auto min =
                      Utils::File::readFileLines(path.value() / "fan1_min");
                  auto max =
                      Utils::File::readFileLines(path.value() / "fan1_max");
                  if (!min.empty() && !max.empty()) {
                    unsigned int minValue;
                    unsigned int maxValue;
                    if (Utils::String::toNumber<unsigned int>(minValue,
                                                              min.front()) &&
                        Utils::String::toNumber<unsigned int>(maxValue,
                                                              max.front()))
                      if (min < max) {
                        range = {
                            units::angular_velocity::revolutions_per_minute_t(
                                minValue),
                            units::angular_velocity::revolutions_per_minute_t(
                                maxValue)};
                      }
                  }
                }

                std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
                dataSources.emplace_back(
                    std::make_unique<SysFSDataSource<unsigned int>>(
                        fanInput,
                        [](std::string const &data, unsigned int &output) {
                          Utils::String::toNumber<unsigned int>(output, data);
                        }));
                dataSources.emplace_back(
                    std::make_unique<SysFSDataSource<unsigned int>>(
                        pwm, [](std::string const &data, unsigned int &output) {
                          Utils::String::toNumber<unsigned int>(output, data);
                        }));

                sensors.emplace_back(
                    std::make_unique<Sensor<units::angular_velocity::revolutions_per_minute_t,
                                            unsigned int>>(
                        AMD::FanSpeedRPM::ItemID, std::move(dataSources),
                        std::move(range),
                        [](std::vector<unsigned int> const &input) {
                          return input[1] > 0 ? input[0] : 0;
                        }));
              }
              else {
                LOG(WARNING)
                    << fmt::format("Unknown data format on {}", pwm.string());
                LOG(ERROR) << fileLines.front().c_str();
              }
            }
          }
          else {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        fanInput.string());
            LOG(ERROR) << fileLines.front().c_str();
          }
        }
      }
    }

    return sensors;
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
