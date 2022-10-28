// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpusensorprovider.h"

#include <utility>

std::vector<std::unique_ptr<ICPUSensorProvider::IProvider>> const &
CPUSensorProvider::cpuSensorProviders() const
{
  return cpuSensorProviders_();
}

std::vector<std::unique_ptr<ICPUSensorProvider::IProvider>> &
CPUSensorProvider::cpuSensorProviders_()
{
  static std::vector<std::unique_ptr<ICPUSensorProvider::IProvider>> providers;
  return providers;
}

bool CPUSensorProvider::registerProvider(
    std::unique_ptr<ICPUSensorProvider::IProvider> &&provider)
{
  cpuSensorProviders_().emplace_back(std::move(provider));
  return true;
}
