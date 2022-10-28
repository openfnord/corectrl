// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/icpucontrolprovider.h"
#include <memory>
#include <vector>

class CPUFreqModeProvider final
: public ICPUControlProvider::IProvider
, public ICPUControlProvider
{
 public:
  std::vector<std::unique_ptr<IControl>>
  provideCPUControls(ICPUInfo const &cpuInfo,
                     ISWInfo const &swInfo) const override;

  std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> const &
  cpuControlProviders() const final override;

  static bool
  registerProvider(std::unique_ptr<ICPUControlProvider::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> &
  providers_();

  static bool const registered_;
};
