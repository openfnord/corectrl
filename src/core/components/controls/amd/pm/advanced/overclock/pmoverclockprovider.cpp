// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverclockprovider.h"

#include "../pmadvancedprovider.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "pmoverclock.h"
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMOverclockProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                             ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && (kernel >= std::make_tuple(4, 8, 0) &&
                               kernel < std::make_tuple(4, 17, 0))) {

      std::vector<std::unique_ptr<IControl>> modeControls;

      for (auto &provider : providers_()) {
        auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
        modeControls.insert(modeControls.end(),
                            std::make_move_iterator(newControls.begin()),
                            std::make_move_iterator(newControls.end()));
      }
      if (!modeControls.empty())
        controls.emplace_back(
            std::make_unique<PMOverclock>(std::move(modeControls)));
    }
  }

  return controls;
}

bool AMD::PMOverclockProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMOverclockProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMOverclockProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMOverclockProvider>());
