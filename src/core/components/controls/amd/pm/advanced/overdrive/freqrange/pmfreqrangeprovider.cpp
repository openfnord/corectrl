// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

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

        auto outOfRangeStates =
            Utils::AMD::ppOdClkVoltageFreqRangeOutOfRangeStates(
                controlName, ppOdClkVoltLines);

        auto controlIsValid =
            !(outOfRangeStates.has_value() && outOfRangeStates->size() > 1) &&
            Utils::AMD::parseOverdriveClkRange(controlName, ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOverdriveClks(controlName, ppOdClkVoltLines)
                .has_value();

        if (controlIsValid) {

          auto controlCmdId =
              Utils::AMD::getOverdriveClkControlCmdId(controlName);
          if (controlCmdId.has_value()) {

            if (outOfRangeStates.has_value())
              LOG(WARNING) << fmt::format(
                  "Detected out of range state index {} on control {}",
                  outOfRangeStates->at(0), controlName);

            auto disabledBound =
                outOfRangeStates.has_value()
                    ? std::optional<AMD::PMFreqRange::DisabledBound>(
                          AMD::PMFreqRange::DisabledBound{outOfRangeStates->at(0)})
                    : std::nullopt;

            controls.emplace_back(std::make_unique<AMD::PMFreqRange>(
                std::move(controlName), std::move(*controlCmdId),
                std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                    ppOdClkVolt),
                std::move(disabledBound)));
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
