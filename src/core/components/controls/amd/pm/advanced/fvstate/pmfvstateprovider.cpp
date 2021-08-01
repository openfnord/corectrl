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
#include "pmfvstateprovider.h"

#include "../pmadvancedprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/components/controls/amd/pm/handlers/ppdpmhandler.h"
#include "core/info/amd/gpuinfopmodcv.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmfvstate.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

std::unique_ptr<IControl>
AMD::PMFVStateProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                          ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && (kernel >= std::make_tuple(4, 18, 0) &&
                               gpuInfo.hasCapability(GPUInfoPMODCV::Fixed))) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
      auto dpmSclk = gpuInfo.path().sys / "pp_dpm_sclk";
      auto dpmMclk = gpuInfo.path().sys / "pp_dpm_mclk";
      if (Utils::File::isSysFSEntryValid(perfLevel) &&
          Utils::File::isSysFSEntryValid(ppOdClkVolt) &&
          Utils::File::isSysFSEntryValid(dpmSclk) &&
          Utils::File::isSysFSEntryValid(dpmMclk)) {

        auto ppOdClkVoltLines = Utils::File::readFileLines(ppOdClkVolt);
        auto ppOdClkVoltValid =
            !Utils::AMD::ppOdClkVoltageHasKnownQuirks(ppOdClkVoltLines) &&
            Utils::AMD::parseOverdriveClkRange("SCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOverdriveClkRange("MCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOverdriveVoltRange(ppOdClkVoltLines).has_value() &&
            Utils::AMD::parseOverdriveClksVolts("SCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOverdriveClksVolts("MCLK", ppOdClkVoltLines)
                .has_value();

        auto dpmSclkLines = Utils::File::readFileLines(dpmSclk);
        auto dpmSclkValid = Utils::AMD::parseDPMStates(dpmSclkLines).has_value();

        auto dpmMclkLines = Utils::File::readFileLines(dpmMclk);
        auto dpmMclkValid = Utils::AMD::parseDPMStates(dpmMclkLines).has_value();

        if (ppOdClkVoltValid && dpmSclkValid && dpmMclkValid) {

          return std::make_unique<AMD::PMFVState>(
              std::make_unique<SysFSDataSource<std::string>>(perfLevel),
              std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                  ppOdClkVolt),
              std::make_unique<PpDpmHandler>(
                  std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                      dpmSclk)),
              std::make_unique<PpDpmHandler>(
                  std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                      dpmMclk)));
        }
        else {
          if (!ppOdClkVoltValid) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        ppOdClkVolt.string());
            for (auto &line : ppOdClkVoltLines)
              LOG(ERROR) << line.c_str();
          }

          if (!dpmSclkValid) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        dpmSclk.string());
            for (auto &line : dpmSclkLines)
              LOG(ERROR) << line.c_str();
          }

          if (!dpmMclkValid) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        dpmMclk.string());
            for (auto &line : dpmMclkLines)
              LOG(ERROR) << line.c_str();
          }
        }
      }
    }
  }

  return nullptr;
}

bool const AMD::PMFVStateProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMFVStateProvider>());
