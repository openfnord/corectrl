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
#include "pmdynamicfreqprovider.h"

#include "../freqmode/pmfreqmodeprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "pmdynamicfreq.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>

std::vector<std::unique_ptr<IControl>>
AMD::PMDynamicFreqProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                               ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && kernel >= std::make_tuple(4, 2, 0)) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      if (Utils::File::isSysFSEntryValid(perfLevel)) {
        controls.emplace_back(std::make_unique<AMD::PMDynamicFreq>(
            std::make_unique<SysFSDataSource<std::string>>(perfLevel)));
      }
    }
  }

  return controls;
}

bool AMD::PMDynamicFreqProvider::register_()
{
  AMD::PMFreqModeProvider::registerProvider(
      std::make_unique<AMD::PMDynamicFreqProvider>());

  return true;
}

bool const AMD::PMDynamicFreqProvider::registered_ =
    AMD::PMDynamicFreqProvider::register_();
