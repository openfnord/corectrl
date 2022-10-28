// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerprofileprovider.h"

#include "../pmadvancedprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmpowerprofile.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

std::vector<std::unique_ptr<IControl>>
AMD::PMPowerProfileProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                                ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && kernel >= std::make_tuple(4, 18, 0)) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      auto profileMode = gpuInfo.path().sys / "pp_power_profile_mode";
      if (Utils::File::isSysFSEntryValid(perfLevel) &&
          Utils::File::isSysFSEntryValid(profileMode)) {

        auto modeLines = Utils::File::readFileLines(profileMode);
        auto modes = Utils::AMD::parsePowerProfileModeModes(modeLines);

        if (modes.has_value())
          controls.emplace_back(std::make_unique<AMD::PMPowerProfile>(
              std::make_unique<SysFSDataSource<std::string>>(perfLevel),
              std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                  profileMode),
              modes.value()));
        else {
          LOG(WARNING) << fmt::format("Unknown data format on {}",
                                      profileMode.string());
          for (auto &line : modeLines)
            LOG(ERROR) << line.c_str();
        }
      }
    }
  }

  return controls;
}

bool const AMD::PMPowerProfileProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMPowerProfileProvider>());
