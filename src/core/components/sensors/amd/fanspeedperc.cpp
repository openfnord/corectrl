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

namespace fs = std::filesystem;

namespace AMD {
namespace FanSpeedPerc {

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

      if ((driver == "amdgpu" && kernel >= std::make_tuple(4, 2, 0) &&
           kernel < std::make_tuple(4, 10, 0)) ||
          (driver == "radeon" && kernel >= std::make_tuple(4, 0, 0))) {

        auto path =
            Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
        if (path.has_value()) {

          auto pwm = path.value() / "pwm1";
          if (Utils::File::isSysFSEntryValid(pwm)) {

            unsigned int value;
            auto pwmLines = Utils::File::readFileLines(pwm);
            if (Utils::String::toNumber<unsigned int>(value, pwmLines.front())) {

              std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
              dataSources.emplace_back(
                  std::make_unique<SysFSDataSource<unsigned int>>(
                      pwm, [](std::string const &data, unsigned int &output) {
                        unsigned int value;
                        Utils::String::toNumber<unsigned int>(value, data);
                        output = value / 2.55;
                      }));

              return std::make_unique<
                  Sensor<units::dimensionless::scalar_t, unsigned int>>(
                  AMD::FanSpeedPerc::ItemID, std::move(dataSources),
                  std::make_pair(units::dimensionless::scalar_t(0),
                                 units::dimensionless::scalar_t(100)));
            }
            else {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          pwm.string());
              LOG(ERROR) << pwmLines.front().c_str();
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
