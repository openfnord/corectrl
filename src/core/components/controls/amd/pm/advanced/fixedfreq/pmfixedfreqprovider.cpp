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
#include "pmfixedfreqprovider.h"

#include "../freqmode/pmfreqmodeprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/controls/amd/pm/handlers/ppdpmhandler.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"

#include "pmfixedfreq.h"

std::unique_ptr<IControl>
AMD::PMFixedFreqProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                            ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && kernel >= std::make_tuple(4, 6, 0)) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      auto dpmSclk = gpuInfo.path().sys / "pp_dpm_sclk";
      auto dpmMclk = gpuInfo.path().sys / "pp_dpm_mclk";
      if (Utils::File::isSysFSEntryValid(perfLevel) &&
          Utils::File::isSysFSEntryValid(dpmSclk) &&
          Utils::File::isSysFSEntryValid(dpmMclk)) {

        return std::make_unique<AMD::PMFixedFreq>(
            std::make_unique<SysFSDataSource<std::string>>(perfLevel),
            std::make_unique<PpDpmHandler>(
                std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                    dpmSclk)),
            std::make_unique<PpDpmHandler>(
                std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                    dpmMclk)));
      }
    }
  }

  return nullptr;
}

bool AMD::PMFixedFreqProvider::register_()
{
  AMD::PMFreqModeProvider::registerProvider(
      std::make_unique<AMD::PMFixedFreqProvider>());

  return true;
}

bool const AMD::PMFixedFreqProvider::registered_ =
    AMD::PMFixedFreqProvider::register_();
