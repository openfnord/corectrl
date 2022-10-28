// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icpuinfo.h"
#include "igpuinfo.h"
#include "iswinfo.h"
#include <memory>
#include <vector>

class InfoProviderRegistry final
: public ICPUInfo::IProviderRegistry
, public IGPUInfo::IProviderRegistry
, public ISWInfo::IProviderRegistry
{
 public:
  std::vector<std::unique_ptr<ICPUInfo::IProvider>> const &
  cpuInfoProviders() const override;
  std::vector<std::unique_ptr<IGPUInfo::IProvider>> const &
  gpuInfoProviders() const override;
  std::vector<std::unique_ptr<ISWInfo::IProvider>> const &
  swInfoProviders() const override;

  static bool add(std::unique_ptr<ICPUInfo::IProvider> &&provider);
  static bool add(std::unique_ptr<IGPUInfo::IProvider> &&provider);
  static bool add(std::unique_ptr<ISWInfo::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<ICPUInfo::IProvider>> &cpuInfoProviders_();
  static std::vector<std::unique_ptr<IGPUInfo::IProvider>> &gpuInfoProviders_();
  static std::vector<std::unique_ptr<ISWInfo::IProvider>> &swInfoProviders_();
};
