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
#include "pmpowerstateprovider.h"

#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/controls/gpucontrolprovider.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "pmpowerstate.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>

std::unique_ptr<IControl>
AMD::PMPowerStateProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                             ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "radeon" && kernel >= std::make_tuple(3, 11, 0)) {
      auto powerDpmStatePath = gpuInfo.path().sys / "power_dpm_state";
      if (Utils::File::isSysFSEntryValid(powerDpmStatePath))
        return std::make_unique<AMD::PMPowerState>(
            std::make_unique<SysFSDataSource<std::string>>(powerDpmStatePath));
    }
  }

  return nullptr;
}

bool const AMD::PMPowerStateProvider::registered_ =
    GPUControlProvider::registerProvider(
        std::make_unique<AMD::PMPowerStateProvider>());
