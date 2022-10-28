// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
  std::vector<std::unique_ptr<ISensor>>
  provideCPUSensors(ICPUInfo const &cpuInfo, ISWInfo const &) const override
  {
    std::vector<std::unique_ptr<ISensor>> sensors;

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
          sensors.emplace_back(
              std::make_unique<Sensor<units::frequency::megahertz_t, unsigned int>>(
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
                  }));
      }
    }

    return sensors;
  }
};

static bool register_()
{
  CPUSensorProvider::registerProvider(std::make_unique<CPUFreqPack::Provider>());

  ProfilePartProvider::registerProvider(CPUFreqPack::ItemID, []() {
    return std::make_unique<GraphItemProfilePart>(CPUFreqPack::ItemID,
                                                  "fuchsia");
  });

  ProfilePartXMLParserProvider::registerProvider(CPUFreqPack::ItemID, []() {
    return std::make_unique<GraphItemXMLParser>(CPUFreqPack::ItemID);
  });

  return true;
}

static bool const registered_ = register_();

} // namespace CPUFreqPack
