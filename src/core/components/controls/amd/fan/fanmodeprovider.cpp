// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanmodeprovider.h"

#include "core/components/controls/gpucontrolprovider.h"
#include "core/components/controls/noop.h"
#include "core/info/igpuinfo.h"
#include "core/info/vendor.h"
#include "fanmode.h"
#include <iterator>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::FanModeProvider::provideGPUControls(IGPUInfo const &gpuInfo,
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
      controls.emplace_back(std::make_unique<FanMode>(std::move(modeControls)));
    }
  }

  return controls;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
AMD::FanModeProvider::gpuControlProviders() const
{
  return providers_();
}

bool AMD::FanModeProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::FanModeProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::FanModeProvider::registered_ =
    GPUControlProvider::registerProvider(
        std::make_unique<AMD::FanModeProvider>());
