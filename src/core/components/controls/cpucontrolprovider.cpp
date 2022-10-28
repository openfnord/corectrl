// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpucontrolprovider.h"

#include <utility>

std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> const &
CPUControlProvider::cpuControlProviders() const
{
  return cpuControlProviders_();
}

std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> &
CPUControlProvider::cpuControlProviders_()
{
  static std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> providers;
  return providers;
}

bool CPUControlProvider::registerProvider(
    std::unique_ptr<ICPUControlProvider::IProvider> &&provider)
{
  cpuControlProviders_().emplace_back(std::move(provider));
  return true;
}
