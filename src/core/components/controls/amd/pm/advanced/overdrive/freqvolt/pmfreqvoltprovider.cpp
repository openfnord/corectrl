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
#include "pmfreqvoltprovider.h"

#include "../pmoverdriveprovider.h"
#include "common/fileutils.h"
#include "core/components/amdutils.h"
#include "core/components/controls/amd/pm/handlers/ppdpmhandler.h"
#include "core/info/amd/gpuinfopmoverdrive.h"
#include "core/info/igpuinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmfreqvolt.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

std::vector<std::unique_ptr<IControl>>
AMD::PMFreqVoltProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                            ISWInfo const &) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD &&
      gpuInfo.hasCapability(GPUInfoPMOverdrive::ClkVolt)) {

    auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
    auto ppOdClkVoltLines = Utils::File::readFileLines(ppOdClkVolt);

    auto const controlNames =
        Utils::AMD::parseOverdriveClkControls(ppOdClkVoltLines);
    if (controlNames.has_value()) {
      bool logPPOdClkVoltContents{false};

      for (auto controlName : controlNames.value()) {
        auto control = controlName;
        std::transform(control.cbegin(), control.cend(), control.begin(),
                       ::tolower);

        auto dpmControl = gpuInfo.path().sys / ("pp_dpm_" + control);
        if (Utils::File::isSysFSEntryValid(dpmControl)) {
          auto dpmLines = Utils::File::readFileLines(dpmControl);

          auto dpmIsValid = Utils::AMD::parseDPMStates(dpmLines).has_value();
          auto controlIsValid =
              Utils::AMD::parseOverdriveClkRange(controlName, ppOdClkVoltLines)
                  .has_value() &&
              Utils::AMD::parseOverdriveVoltRange(ppOdClkVoltLines).has_value() &&
              Utils::AMD::parseOverdriveClksVolts(controlName, ppOdClkVoltLines)
                  .has_value();

          if (controlIsValid && dpmIsValid) {

            auto controlCmdId =
                Utils::AMD::getOverdriveClkControlCmdId(controlName);
            if (controlCmdId.has_value()) {

              controls.emplace_back(std::make_unique<AMD::PMFreqVolt>(
                  std::move(controlName), std::move(*controlCmdId),
                  std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                      ppOdClkVolt),
                  std::make_unique<PpDpmHandler>(
                      std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                          dpmControl))));
            }
            else {
              LOG(WARNING) << fmt::format("Unsupported control {}", controlName);
            }
          }
          else {
            if (!controlIsValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          ppOdClkVolt.string());
              logPPOdClkVoltContents = true;
            }

            if (!dpmIsValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          dpmControl.string());
              for (auto &line : dpmLines)
                LOG(ERROR) << line.c_str();
            }
          }
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

bool const AMD::PMFreqVoltProvider::registered_ =
    AMD::PMOverdriveProvider::registerProvider(
        std::make_unique<AMD::PMFreqVoltProvider>());
