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
#include "pmvoltcurveprovider.h"

#include "../pmoverdriveprovider.h"
#include "common/fileutils.h"
#include "core/components/amdutils.h"
#include "core/info/amd/gpuinfopmoverdrive.h"
#include "core/info/igpuinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmvoltcurve.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

std::vector<std::unique_ptr<IControl>>
AMD::PMVoltCurveProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                             ISWInfo const &) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD &&
      gpuInfo.hasCapability(GPUInfoPMOverdrive::VoltCurve)) {

    auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
    auto ppOdClkVoltLines = Utils::File::readFileLines(ppOdClkVolt);

    auto voltCurveControlValid =
        Utils::AMD::parseOverdriveVoltCurveRange(ppOdClkVoltLines).has_value() &&
        Utils::AMD::parseOverdriveVoltCurve(ppOdClkVoltLines).has_value();

    if (voltCurveControlValid) {

      controls.emplace_back(std::make_unique<AMD::PMVoltCurve>(
          "vc", std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                    ppOdClkVolt)));
    }
    else {
      LOG(WARNING) << fmt::format("Invalid data on {}", ppOdClkVolt.string());
      for (auto &line : ppOdClkVoltLines)
        LOG(ERROR) << line.c_str();
    }
  }

  return controls;
}

bool const AMD::PMVoltCurveProvider::registered_ =
    AMD::PMOverdriveProvider::registerProvider(
        std::make_unique<AMD::PMVoltCurveProvider>());
