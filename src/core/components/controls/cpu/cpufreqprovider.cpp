// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpufreqprovider.h"

#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/icpuinfo.h"
#include "core/sysfsdatasource.h"
#include "cpufreq.h"
#include "cpufreqmodeprovider.h"
#include <algorithm>
#include <filesystem>
#include <utility>

std::vector<std::unique_ptr<IControl>>
CPUFreqProvider::provideCPUControls(ICPUInfo const &cpuInfo, ISWInfo const &) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (Utils::File::isDirectoryPathValid("/sys/devices/system/cpu/cpufreq")) {

    auto &executionUnits = cpuInfo.executionUnits();
    if (!executionUnits.empty()) {

      auto governors = availableGovernors(cpuInfo);
      if (!governors.empty()) {

        auto governor = defatultGovernor(cpuInfo, governors);
        auto scalingGovernorDataSources =
            createScalingGovernorDataSources(cpuInfo);

        if (!scalingGovernorDataSources.empty())
          controls.emplace_back(
              std::make_unique<CPUFreq>(std::move(governors), governor,
                                        std::move(scalingGovernorDataSources)));
      }
    }
  }

  return controls;
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

std::string CPUFreqProvider::defatultGovernor(
    ICPUInfo const &cpuInfo, std::vector<std::string> const &governors) const
{
  std::string scalingDriverPath{"cpufreq/scaling_driver"};

  // get scaling driver from the first execution unit
  auto unitScalingDriverPath = cpuInfo.executionUnits().front().sysPath /
                               scalingDriverPath;

  if (Utils::File::isSysFSEntryValid(unitScalingDriverPath)) {
    auto lines = Utils::File::readFileLines(unitScalingDriverPath);
    if (!lines.empty()) {

      std::string governor("ondemand");

      auto driver = lines.front();
      if (driver == "intel_pstate")
        governor = "powersave";

      // clamp governor into available governors
      auto iter = std::find_if(governors.cbegin(), governors.cend(),
                               [&](auto &availableGovernor) {
                                 return governor == availableGovernor;
                               });
      if (iter == governors.cend()) // fallback to first available governor
        governor = governors.front();

      return governor;
    }
  }

  return governors.front();
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
    CPUFreqModeProvider::registerProvider(std::make_unique<CPUFreqProvider>());
