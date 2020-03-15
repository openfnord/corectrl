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
#include "cpufreqpack.h"

#include "../cpusensorprovider.h"
#include "../graphitemprofilepart.h"
#include "../graphitemxmlparser.h"
#include "../sensor.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/idatasource.h"
#include "core/info/icpuinfo.h"
#include "core/iprofilepart.h"
#include "core/iprofilepartxmlparser.h"
#include "core/profilepartprovider.h"
#include "core/profilepartxmlparserprovider.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "units/units.h"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace CPUFreqPack {

class Provider final : public ICPUSensorProvider::IProvider
{
 public:
  std::unique_ptr<ISensor> provideCPUSensor(ICPUInfo const &cpuInfo,
                                            ISWInfo const &) const override
  {
    if (Utils::File::isDirectoryPathValid("/sys/devices/system/cpu/cpufreq")) {

      auto &executionUnits = cpuInfo.executionUnits();
      if (!executionUnits.empty()) {
        std::optional<
            std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
            range;

        auto minFreqPath = executionUnits.front().sysPath /
                           "cpufreq/cpuinfo_min_freq";
        auto maxFreqPath = executionUnits.front().sysPath /
                           "cpufreq/cpuinfo_max_freq";
        if (Utils::File::isSysFSEntryValid(minFreqPath) &&
            Utils::File::isSysFSEntryValid(maxFreqPath)) {
          auto minFreqLines = Utils::File::readFileLines(minFreqPath);
          auto maxFreqLines = Utils::File::readFileLines(maxFreqPath);

          unsigned int minFreq{0};
          unsigned int maxFreq{0};
          if (Utils::String::toNumber<unsigned int>(minFreq,
                                                    minFreqLines.front()) &&
              Utils::String::toNumber<unsigned int>(maxFreq,
                                                    maxFreqLines.front())) {
            if (minFreq < maxFreq)
              range = {units::frequency::kilohertz_t(minFreq),
                       units::frequency::kilohertz_t(maxFreq)};
          }
        }

        std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
        for (auto &executionUnit : cpuInfo.executionUnits()) {
          auto curFreqPath = executionUnit.sysPath / "cpufreq/scaling_cur_freq";
          if (Utils::File::isSysFSEntryValid(curFreqPath)) {

            unsigned int value;
            auto curFreqLines = Utils::File::readFileLines(curFreqPath);
            if (Utils::String::toNumber<unsigned int>(value,
                                                      curFreqLines.front())) {
              dataSources.emplace_back(
                  std::make_unique<SysFSDataSource<unsigned int>>(
                      curFreqPath,
                      [](std::string const &data, unsigned int &output) {
                        Utils::String::toNumber<unsigned int>(output, data);
                      }));
            }
            else {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          curFreqPath.string());
              LOG(ERROR) << curFreqLines.front().c_str();
            }
          }
        }

        if (!dataSources.empty())
          return std::make_unique<Sensor<units::frequency::megahertz_t, unsigned int>>(
              CPUFreqPack::ItemID, std::move(dataSources), std::move(range),
              [](std::vector<unsigned int> const &input) {
                auto maxIter = std::max_element(input.cbegin(), input.cend());
                if (maxIter != input.cend()) {
                  units::frequency::kilohertz_t maxKHz(*maxIter);
                  return maxKHz.convert<units::frequency::megahertz>()
                      .to<unsigned int>();
                }
                else
                  return 0u;
              });
      }
    }

    return nullptr;
  }
};

static bool register_()
{
  CPUSensorProvider::registerProvider(std::make_unique<CPUFreqPack::Provider>());

  ProfilePartProvider::registerProvider(CPUFreqPack::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(CPUFreqPack::ItemID,
                                                  "darkorange");
  });

  ProfilePartXMLParserProvider::registerProvider(CPUFreqPack::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(CPUFreqPack::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace CPUFreqPack
