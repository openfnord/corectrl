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
#include "fanspeedperc.h"

#include "../gpusensorprovider.h"
#include "../graphitemprofilepart.h"
#include "../graphitemxmlparser.h"
#include "../sensor.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/info/vendor.h"
#include "core/iprofilepart.h"
#include "core/iprofilepartxmlparser.h"
#include "core/profilepartprovider.h"
#include "core/profilepartxmlparserprovider.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "units/units.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace AMD {
namespace FanSpeedPerc {

// NOTE FanSpeedPerc is a last resort fan speed sensor.
//
// It will be used only on systems without proper support of fan1_input
// (typically old hardware using radeon driver and some specific hardware supported
// by the amdgpu driver).
//
// See #184.

class Provider final : public IGPUSensorProvider::IProvider
{
 public:
  std::vector<std::unique_ptr<ISensor>>
  provideGPUSensors(IGPUInfo const &gpuInfo, ISWInfo const &) const override
  {
    std::vector<std::unique_ptr<ISensor>> sensors;

    if (gpuInfo.vendor() == Vendor::AMD) {

      auto path =
          Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
      if (path.has_value()) {

        auto pwm = path.value() / "pwm1";
        if (Utils::File::isSysFSEntryValid(pwm)) {

          unsigned int value;
          auto fileLines = Utils::File::readFileLines(pwm);
          if (Utils::String::toNumber<unsigned int>(value, fileLines.front())) {

            // Prefer fanspeedrpm control over this one if the former it's available on the system
            auto fanInput = path.value() / "fan1_input";
            if (!(Utils::File::isSysFSEntryValid(fanInput) &&
                  Utils::String::toNumber<unsigned int>(
                      value, Utils::File::readFileLines(fanInput).front()))) {

              std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
              dataSources.emplace_back(
                  std::make_unique<SysFSDataSource<unsigned int>>(
                      pwm, [](std::string const &data, unsigned int &output) {
                        unsigned int value;
                        Utils::String::toNumber<unsigned int>(value, data);
                        output = value / 2.55;
                      }));

              sensors.emplace_back(
                  std::make_unique<
                      Sensor<units::dimensionless::scalar_t, unsigned int>>(
                      AMD::FanSpeedPerc::ItemID, std::move(dataSources),
                      std::make_pair(units::dimensionless::scalar_t(0),
                                     units::dimensionless::scalar_t(100))));
            }
          }
          else {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        pwm.string());
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
      std::make_unique<AMD::FanSpeedPerc::Provider>());

  ProfilePartProvider::registerProvider(AMD::FanSpeedPerc::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::FanSpeedPerc::ItemID,
                                                  "lightskyblue");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::FanSpeedPerc::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::FanSpeedPerc::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace FanSpeedPerc
} // namespace AMD
