// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerstatemodeprovider.h"

#include "core/components/controls/gpucontrolprovider.h"
#include "core/components/controls/noop.h"
#include "core/info/igpuinfo.h"
#include "pmpowerstatemode.h"
#include <iterator>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMPowerStateModeProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                                  ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    std::vector<std::unique_ptr<IControl>> modeControls;

    for (auto &provider : gpuControlProviders()) {
      auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
      modeControls.insert(modeControls.end(),
                          std::make_move_iterator(newControls.begin()),
                          std::make_move_iterator(newControls.end()));
    }

    if (!modeControls.empty()) {
      modeControls.emplace_back(std::make_unique<Noop>());
      controls.emplace_back(
          std::make_unique<PMPowerStateMode>(std::move(modeControls)));
    }
  }

  return controls;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
AMD::PMPowerStateModeProvider::gpuControlProviders() const
{
  return providers_();
}

bool AMD::PMPowerStateModeProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMPowerStateModeProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMPowerStateModeProvider::registered_ =
    GPUControlProvider::registerProvider(
        std::make_unique<AMD::PMPowerStateModeProvider>());
