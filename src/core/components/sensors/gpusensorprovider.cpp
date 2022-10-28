// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpusensorprovider.h"

#include <utility>

std::vector<std::unique_ptr<IGPUSensorProvider::IProvider>> const &
GPUSensorProvider::gpuSensorProviders() const
{
  return gpuSensorProviders_();
}

std::vector<std::unique_ptr<IGPUSensorProvider::IProvider>> &
GPUSensorProvider::gpuSensorProviders_()
{
  static std::vector<std::unique_ptr<IGPUSensorProvider::IProvider>> providers;
  return providers;
}

bool GPUSensorProvider::registerProvider(
    std::unique_ptr<IGPUSensorProvider::IProvider> &&provider)
{
  gpuSensorProviders_().emplace_back(std::move(provider));
  return true;
}
