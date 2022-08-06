//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "pmfreqrangeprovider.h"

#include "../pmoverdriveprovider.h"
#include "common/fileutils.h"
#include "core/components/amdutils.h"
#include "core/info/amd/gpuinfopmoverdrive.h"
#include "core/info/igpuinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmfreqrange.h"
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

std::vector<std::unique_ptr<IControl>>
AMD::PMFreqRangeProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                             ISWInfo const &) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD &&
      gpuInfo.hasCapability(GPUInfoPMOverdrive::Clk)) {

    auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
    auto ppOdClkVoltLines = Utils::File::readFileLines(ppOdClkVolt);

    auto const controlNames =
        Utils::AMD::parseOverdriveClkControls(ppOdClkVoltLines);
    if (controlNames.has_value()) {
      bool logPPOdClkVoltContents{false};

      for (auto controlName : controlNames.value()) {

        auto controlIsValid =
            !Utils::AMD::ppOdClkVoltageHasKnownFreqRangeQuirks(
                controlName, ppOdClkVoltLines) &&
            Utils::AMD::parseOverdriveClkRange(controlName, ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOverdriveClks(controlName, ppOdClkVoltLines)
                .has_value();

        if (controlIsValid) {

          auto controlCmdId =
              Utils::AMD::getOverdriveClkControlCmdId(controlName);
          if (controlCmdId.has_value()) {

            controls.emplace_back(std::make_unique<AMD::PMFreqRange>(
                std::move(controlName), std::move(*controlCmdId),
                std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                    ppOdClkVolt)));
          }
          else {
            LOG(WARNING) << fmt::format("Unsupported control {}", controlName);
          }
        }
        else {
          LOG(WARNING) << fmt::format("Invalid data on {} for control {}",
                                      ppOdClkVolt.string(), controlName);
          logPPOdClkVoltContents = true;
        }
      }

      if (logPPOdClkVoltContents) {
        for (auto &line : ppOdClkVoltLines)
          LOG(ERROR) << line.c_str();
      }
    }
  }

  return controls;
}

bool const AMD::PMFreqRangeProvider::registered_ =
    AMD::PMOverdriveProvider::registerProvider(
        std::make_unique<AMD::PMFreqRangeProvider>());
