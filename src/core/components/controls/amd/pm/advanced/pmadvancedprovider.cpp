// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmadvancedprovider.h"

#include "../pmperfmodeprovider.h"
#include "core/info/igpuinfo.h"
#include "pmadvanced.h"
#include <iterator>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMAdvancedProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                            ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    std::vector<std::unique_ptr<IControl>> groupControls;

    for (auto &provider : providers_()) {
      auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
      groupControls.insert(groupControls.end(),
                           std::make_move_iterator(newControls.begin()),
                           std::make_move_iterator(newControls.end()));
    }
    if (!groupControls.empty())
      controls.emplace_back(
          std::make_unique<PMAdvanced>(std::move(groupControls)));
  }

  return controls;
}

bool AMD::PMAdvancedProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMAdvancedProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMAdvancedProvider::registered_ =
    AMD::PMPerfModeProvider::registerProvider(
        std::make_unique<AMD::PMAdvancedProvider>());
