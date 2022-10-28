// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/igpucontrolprovider.h"
#include <memory>
#include <vector>

namespace AMD {

class PMFreqModeProvider final
: public IGPUControlProvider::IProvider
, public IGPUControlProvider
{
 public:
  std::vector<std::unique_ptr<IControl>>
  provideGPUControls(IGPUInfo const &gpuInfo,
                     ISWInfo const &swInfo) const override;

  std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
  gpuControlProviders() const final override;

  static bool
  registerProvider(std::unique_ptr<IGPUControlProvider::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
  providers_();

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
