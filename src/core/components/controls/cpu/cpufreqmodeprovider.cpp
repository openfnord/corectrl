// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpufreqmodeprovider.h"

#include "core/components/controls/cpucontrolprovider.h"
#include "core/components/controls/noop.h"
#include "core/info/icpuinfo.h"
#include "cpufreqmode.h"

std::vector<std::unique_ptr<IControl>>
CPUFreqModeProvider::provideCPUControls(ICPUInfo const &cpuInfo,
                                        ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;
  std::vector<std::unique_ptr<IControl>> modeControls;

  for (auto &provider : cpuControlProviders()) {
    auto newControls = provider->provideCPUControls(cpuInfo, swInfo);
    modeControls.insert(modeControls.end(),
                        std::make_move_iterator(newControls.begin()),
                        std::make_move_iterator(newControls.end()));
  }

  if (!modeControls.empty()) {
    modeControls.emplace_back(std::make_unique<Noop>());
    controls.emplace_back(std::make_unique<CPUFreqMode>(std::move(modeControls)));
  }

  return controls;
}

std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> const &
CPUFreqModeProvider::cpuControlProviders() const
{
  return providers_();
}

bool CPUFreqModeProvider::registerProvider(
    std::unique_ptr<ICPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> &
CPUFreqModeProvider::providers_()
{
  static std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> providers;
  return providers;
}

bool const CPUFreqModeProvider::registered_ =
    CPUControlProvider::registerProvider(std::make_unique<CPUFreqModeProvider>());
