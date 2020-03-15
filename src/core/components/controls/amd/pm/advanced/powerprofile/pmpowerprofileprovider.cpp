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

std::unique_ptr<IControl>
AMD::PMPowerProfileProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                               ISWInfo const &swInfo) const
{
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
          return std::make_unique<AMD::PMPowerProfile>(
              std::make_unique<SysFSDataSource<std::string>>(perfLevel),
              std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                  profileMode),
              modes.value());
        else {
          LOG(WARNING) << fmt::format("Unknown data format on {}",
                                      profileMode.string());
          for (auto &line : modeLines)
            LOG(ERROR) << line.c_str();
        }
      }
    }
  }

  return nullptr;
}

bool const AMD::PMPowerProfileProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMPowerProfileProvider>());
