// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverdriveprovider.h"

#include "../pmadvancedprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmoverdrive.h"
#include <filesystem>
#include <iterator>
#include <tuple>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMOverdriveProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                             ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && (kernel >= std::make_tuple(4, 18, 0))) {

      auto perfLevel = gpuInfo.path().sys / "power_dpm_force_performance_level";
      auto ppOdClkVolt = gpuInfo.path().sys / "pp_od_clk_voltage";
      if (Utils::File::isSysFSEntryValid(perfLevel) &&
          Utils::File::isSysFSEntryValid(ppOdClkVolt)) {

        std::vector<std::unique_ptr<IControl>> groupControls;
        for (auto &provider : providers_()) {
          auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
          groupControls.insert(groupControls.end(),
                               std::make_move_iterator(newControls.begin()),
                               std::make_move_iterator(newControls.end()));
        }
        if (!groupControls.empty())
          controls.emplace_back(std::make_unique<PMOverdrive>(
              std::make_unique<SysFSDataSource<std::string>>(perfLevel),
              std::make_unique<SysFSDataSource<std::vector<std::string>>>(
                  ppOdClkVolt),
              std::move(groupControls)));
      }
    }
  }

  return controls;
}

bool AMD::PMOverdriveProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMOverdriveProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMOverdriveProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMOverdriveProvider>());
