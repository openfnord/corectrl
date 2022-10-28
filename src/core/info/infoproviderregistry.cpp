// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "infoproviderregistry.h"

#include <utility>

std::vector<std::unique_ptr<ICPUInfo::IProvider>> const &
InfoProviderRegistry::cpuInfoProviders() const
{
  return cpuInfoProviders_();
}

std::vector<std::unique_ptr<IGPUInfo::IProvider>> const &
InfoProviderRegistry::gpuInfoProviders() const
{
  return gpuInfoProviders_();
}

bool InfoProviderRegistry::add(std::unique_ptr<ICPUInfo::IProvider> &&provider)
{
  cpuInfoProviders_().emplace_back(std::move(provider));
  return true;
}

bool InfoProviderRegistry::add(std::unique_ptr<IGPUInfo::IProvider> &&provider)
{
  gpuInfoProviders_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<ISWInfo::IProvider>> const &
InfoProviderRegistry::swInfoProviders() const
{
  return swInfoProviders_();
}

bool InfoProviderRegistry::add(std::unique_ptr<ISWInfo::IProvider> &&provider)
{
  swInfoProviders_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<ICPUInfo::IProvider>> &
InfoProviderRegistry::cpuInfoProviders_()
{
  static std::vector<std::unique_ptr<ICPUInfo::IProvider>> providers;
  return providers;
}

std::vector<std::unique_ptr<IGPUInfo::IProvider>> &
InfoProviderRegistry::gpuInfoProviders_()
{
  static std::vector<std::unique_ptr<IGPUInfo::IProvider>> providers;
  return providers;
}

std::vector<std::unique_ptr<ISWInfo::IProvider>> &
InfoProviderRegistry::swInfoProviders_()
{
  static std::vector<std::unique_ptr<ISWInfo::IProvider>> providers;
  return providers;
}
