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
#include "cpufreqprovider.h"

#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/controls/cpucontrolprovider.h"
#include "core/info/icpuinfo.h"

#include "cpufreq.h"

std::unique_ptr<IControl>
CPUFreqProvider::provideCPUControl(ICPUInfo const &cpuInfo, ISWInfo const &) const
{
  if (Utils::File::isDirectoryPathValid("/sys/devices/system/cpu/cpufreq")) {

    auto &executionUnits = cpuInfo.executionUnits();
    if (!executionUnits.empty()) {
      auto governors = availableGovernors(cpuInfo);
      if (!governors.empty()) {

        auto scalingGovernorDataSources =
            createScalingGovernorDataSources(cpuInfo);
        if (!scalingGovernorDataSources.empty())
          return std::make_unique<CPUFreq>(
              std::move(governors), std::move(scalingGovernorDataSources));
      }
    }
  }

  return nullptr;
}

std::vector<std::string>
CPUFreqProvider::availableGovernors(ICPUInfo const &cpuInfo) const
{
  std::string availableGovernorsPath{"cpufreq/scaling_available_governors"};

  // get available governors from the first execution unit
  auto unitAvailableGovernorsPath = cpuInfo.executionUnits().front().sysPath /
                                    availableGovernorsPath;

  if (Utils::File::isSysFSEntryValid(unitAvailableGovernorsPath)) {
    auto lines = Utils::File::readFileLines(unitAvailableGovernorsPath);
    return Utils::String::split(lines.front());
  }
  else
    return {};
}

std::vector<std::unique_ptr<IDataSource<std::string>>>
CPUFreqProvider::createScalingGovernorDataSources(ICPUInfo const &cpuInfo) const
{
  std::vector<std::unique_ptr<IDataSource<std::string>>> scalingGovernorDataSources;

  std::string scalingGovernorPath{"cpufreq/scaling_governor"};
  for (auto &executionUnit : cpuInfo.executionUnits()) {
    auto unitScalingGovernorPath = executionUnit.sysPath / scalingGovernorPath;
    if (Utils::File::isSysFSEntryValid(unitScalingGovernorPath))
      scalingGovernorDataSources.emplace_back(
          std::make_unique<SysFSDataSource<std::string>>(executionUnit.sysPath /
                                                         scalingGovernorPath));
  }

  return scalingGovernorDataSources;
}

bool const CPUFreqProvider::registered_ =
    CPUControlProvider::registerProvider(std::make_unique<CPUFreqProvider>());
