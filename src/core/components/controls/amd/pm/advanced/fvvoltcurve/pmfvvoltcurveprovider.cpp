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
#include "pmfvvoltcurveprovider.h"

#include "../pmadvancedprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/info/amd/gpuinfopmodcv.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmfvvoltcurve.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

std::unique_ptr<IControl>
AMD::PMFVVoltCurveProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                              ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && (kernel >= std::make_tuple(4, 18, 0) &&
                               gpuInfo.hasCapability(GPUInfoPMODCV::Curve))) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
      if (Utils::File::isSysFSEntryValid(perfLevel) &&
          Utils::File::isSysFSEntryValid(ppOdClkVolt)) {

        auto ppOdClkVoltLines = Utils::File::readFileLines(ppOdClkVolt);
        auto ppOdClkVoltValid =
            !Utils::AMD::ppOdClkVoltageHasKnownQuirks(ppOdClkVoltLines) &&
            Utils::AMD::parseOdClkVoltStateClkRange("SCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOdClkVoltStateClkRange("MCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOdClkVoltCurveVoltRange(ppOdClkVoltLines).has_value() &&
            Utils::AMD::parseOdClkVoltCurveStates("SCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOdClkVoltCurveStates("MCLK", ppOdClkVoltLines)
                .has_value() &&
            Utils::AMD::parseOdClkVoltCurvePoints(ppOdClkVoltLines).has_value();

        if (ppOdClkVoltValid) {

          return std::make_unique<AMD::PMFVVoltCurve>(
              std::make_unique<SysFSDataSource<std::string>>(perfLevel),
              std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                  ppOdClkVolt));
        }
        else {
          LOG(WARNING) << fmt::format("Invalid data on {}", ppOdClkVolt.string());
          for (auto &line : ppOdClkVoltLines)
            LOG(ERROR) << line.c_str();
        }
      }
    }
  }

  return nullptr;
}

bool const AMD::PMFVVoltCurveProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMFVVoltCurveProvider>());
