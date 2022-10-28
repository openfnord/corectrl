// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmvoltoffsetprovider.h"

#include "../pmoverdriveprovider.h"
#include "common/fileutils.h"
#include "core/components/amdutils.h"
#include "core/info/amd/gpuinfopmoverdrive.h"
#include "core/info/igpuinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmvoltoffset.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

std::vector<std::unique_ptr<IControl>>
AMD::PMVoltOffsetProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                              ISWInfo const &) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD &&
      gpuInfo.hasCapability(GPUInfoPMOverdrive::VoltOffset)) {

    auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
    auto ppOdClkVoltLines = Utils::File::readFileLines(ppOdClkVolt);

    auto controlIsValid =
        Utils::AMD::parseOverdriveVoltOffset(ppOdClkVoltLines).has_value();

    if (controlIsValid) {

      controls.emplace_back(std::make_unique<AMD::PMVoltOffset>(
          std::make_unique<SysFSDataSource<std::vector<std::string>>>(
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

bool const AMD::PMVoltOffsetProvider::registered_ =
    AMD::PMOverdriveProvider::registerProvider(
        std::make_unique<AMD::PMVoltOffsetProvider>());
