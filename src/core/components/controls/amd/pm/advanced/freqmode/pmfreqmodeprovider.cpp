// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqmodeprovider.h"

#include "../overclock/pmoverclockprovider.h"
#include "core/info/igpuinfo.h"
#include "pmfreqmode.h"
#include <iterator>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMFreqModeProvider::provideGPUControls(IGPUInfo const &gpuInfo,
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
    if (!modeControls.empty())
      controls.emplace_back(
          std::make_unique<PMFreqMode>(std::move(modeControls)));
  }

  return controls;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
AMD::PMFreqModeProvider::gpuControlProviders() const
{
  return providers_();
}

bool AMD::PMFreqModeProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMFreqModeProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool AMD::PMFreqModeProvider::register_()
{
  PMOverclockProvider::registerProvider(
      std::make_unique<AMD::PMFreqModeProvider>());

  return true;
}

bool const AMD::PMFreqModeProvider::registered_ =
    AMD::PMFreqModeProvider::register_();
