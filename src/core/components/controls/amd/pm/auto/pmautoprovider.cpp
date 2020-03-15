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

std::unique_ptr<IControl>
AMD::PMAutoProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                       ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));

    if (gpuInfo.hasCapability(GPUInfoPM::Legacy) &&
        kernel >= std::make_tuple(3, 0, 0)) {

      auto powerMethod = gpuInfo.path().sys / "power_method";
      auto powerProfile = gpuInfo.path().sys / "power_profile";
      if (Utils::File::isSysFSEntryValid(powerMethod) &&
          Utils::File::isSysFSEntryValid(powerProfile)) {

        return std::make_unique<AMD::PMAutoLegacy>(
            std::make_unique<SysFSDataSource<std::string>>(powerMethod),
            std::make_unique<SysFSDataSource<std::string>>(powerProfile));
      }
    }
    else if ((gpuInfo.hasCapability(GPUInfoPM::Radeon) &&
              kernel >= std::make_tuple(3, 11, 0)) ||
             (gpuInfo.hasCapability(GPUInfoPM::Amdgpu) &&
              kernel >= std::make_tuple(4, 2, 0))) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      if (Utils::File::isSysFSEntryValid(perfLevel)) {

        return std::make_unique<AMD::PMAutoR600>(
            std::make_unique<SysFSDataSource<std::string>>(perfLevel));
      }
    }
  }

  return nullptr;
}

bool const AMD::PMAutoProvider::registered_ =
    AMD::PMPerfModeProvider::registerProvider(
        std::make_unique<AMD::PMAutoProvider>());
