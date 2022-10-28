// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icpucontrolprovider.h"
#include <memory>
#include <vector>

class CPUControlProvider final : public ICPUControlProvider
{
 public:
  std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> const &
  cpuControlProviders() const override;

  static bool
  registerProvider(std::unique_ptr<ICPUControlProvider::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> &
  cpuControlProviders_();
};
