// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "igpucontrolprovider.h"
#include <memory>
#include <vector>

class GPUControlProvider final : public IGPUControlProvider
{
 public:
  std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
  gpuControlProviders() const override;

  static bool
  registerProvider(std::unique_ptr<IGPUControlProvider::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
  gpuControlProviders_();
};
