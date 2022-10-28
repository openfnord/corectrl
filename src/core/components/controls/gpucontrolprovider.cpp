// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpucontrolprovider.h"

#include <utility>

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
GPUControlProvider::gpuControlProviders() const
{
  return gpuControlProviders_();
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
GPUControlProvider::gpuControlProviders_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool GPUControlProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  gpuControlProviders_().emplace_back(std::move(provider));
  return true;
}
