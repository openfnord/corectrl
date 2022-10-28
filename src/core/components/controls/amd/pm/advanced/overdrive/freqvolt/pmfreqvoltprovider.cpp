// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

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

    auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
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
              !Utils::AMD::ppOdClkVoltageHasKnownFreqVoltQuirks(
                  controlName, ppOdClkVoltLines) &&
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
                      std::make_unique<SysFSDataSource<std::string>>(perfLevel),
                      std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                          dpmControl))));
            }
            else {
              LOG(WARNING) << fmt::format("Unsupported control {}", controlName);
            }
          }
          else {
            if (!controlIsValid) {
              LOG(WARNING) << fmt::format("Invalid data on {} for control {}",
                                          ppOdClkVolt.string(), controlName);
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
