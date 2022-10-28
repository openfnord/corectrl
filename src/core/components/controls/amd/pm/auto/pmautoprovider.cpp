// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmautoprovider.h"

#include "../pmperfmodeprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/amd/gpuinfopm.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "pmautolegacy.h"
#include "pmautor600.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>

std::vector<std::unique_ptr<IControl>>
AMD::PMAutoProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                        ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));

    if (gpuInfo.hasCapability(GPUInfoPM::Legacy) &&
        kernel >= std::make_tuple(3, 0, 0)) {

      auto powerMethod = gpuInfo.path().sys / "power_method";
      auto powerProfile = gpuInfo.path().sys / "power_profile";
      if (Utils::File::isSysFSEntryValid(powerMethod) &&
          Utils::File::isSysFSEntryValid(powerProfile)) {

        controls.emplace_back(std::make_unique<AMD::PMAutoLegacy>(
            std::make_unique<SysFSDataSource<std::string>>(powerMethod),
            std::make_unique<SysFSDataSource<std::string>>(powerProfile)));
      }
    }
    else if ((gpuInfo.hasCapability(GPUInfoPM::Radeon) &&
              kernel >= std::make_tuple(3, 11, 0)) ||
             (gpuInfo.hasCapability(GPUInfoPM::Amdgpu) &&
              kernel >= std::make_tuple(4, 2, 0))) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      if (Utils::File::isSysFSEntryValid(perfLevel)) {

        controls.emplace_back(std::make_unique<AMD::PMAutoR600>(
            std::make_unique<SysFSDataSource<std::string>>(perfLevel)));
      }
    }
  }

  return controls;
}

bool const AMD::PMAutoProvider::registered_ =
    AMD::PMPerfModeProvider::registerProvider(
        std::make_unique<AMD::PMAutoProvider>());
