// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuvolt.h"

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
namespace GPUVolt {

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

      if ((driver == "amdgpu" && kernel >= std::make_tuple(4, 1, 7)) ||
          (driver == "radeon" && kernel >= std::make_tuple(5, 1, 1))) {

        auto path =
            Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
        if (path.has_value()) {

          auto voltInput = path.value() / "in0_input";
          if (Utils::File::isSysFSEntryValid(voltInput)) {

            int value;
            auto voltInputLines = Utils::File::readFileLines(voltInput);

            if (Utils::String::toNumber<int>(value, voltInputLines.front())) {

              // auto scaling range
              std::optional<std::pair<units::voltage::millivolt_t,
                                      units::voltage::millivolt_t>>
                  range;

              std::vector<std::unique_ptr<IDataSource<int>>> dataSources;
              dataSources.emplace_back(std::make_unique<SysFSDataSource<int>>(
                  voltInput, [](std::string const &data, int &output) {
                    Utils::String::toNumber<int>(output, data);
                  }));

              sensors.emplace_back(
                  std::make_unique<Sensor<units::voltage::millivolt_t, int>>(
                      AMD::GPUVolt::ItemID, std::move(dataSources),
                      std::move(range)));
            }
            else {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          voltInput.string());
              LOG(ERROR) << voltInputLines.front().c_str();
            }
          }
        }
      }
    }

    return sensors;
  }
};

static bool register_()
{
  GPUSensorProvider::registerProvider(std::make_unique<AMD::GPUVolt::Provider>());

  ProfilePartProvider::registerProvider(AMD::GPUVolt::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(AMD::GPUVolt::ItemID,
                                                  "darkorange");
  });

  ProfilePartXMLParserProvider::registerProvider(AMD::GPUVolt::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(AMD::GPUVolt::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace GPUVolt
} // namespace AMD
