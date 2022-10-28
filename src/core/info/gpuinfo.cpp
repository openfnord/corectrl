// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuinfo.h"

#include <algorithm>
#include <iterator>
#include <utility>

GPUInfo::GPUInfo(Vendor vendor, int index, IGPUInfo::Path &&path) noexcept
: vendor_(vendor)
, index_(index)
, path_(std::move(path))
{
}

Vendor GPUInfo::vendor() const
{
  return vendor_;
}

int GPUInfo::index() const
{
  return index_;
}

IGPUInfo::Path const &GPUInfo::path() const
{
  return path_;
}

std::vector<std::string> GPUInfo::keys() const
{
  std::vector<std::string> keys;
  keys.reserve(info_.size());

  auto const keySelector = [](auto &pair) { return pair.first; };
  std::transform(info_.cbegin(), info_.cend(), std::back_inserter(keys),
                 keySelector);

  return keys;
}

std::string GPUInfo::info(std::string_view key) const
{
  auto const dataIt = info_.find(std::string(key));
  if (dataIt != info_.cend())
    return dataIt->second;

  return std::string{};
}

bool GPUInfo::hasCapability(std::string_view name) const
{
  return capabilities_.count(std::string(name)) > 0;
}

void GPUInfo::initialize(
    std::vector<std::unique_ptr<IGPUInfo::IProvider>> const &providers,
    IHWIDTranslator const &hwidTranslator)
{
  for (auto &provider : providers) {
    auto infos = provider->provideInfo(vendor_, index_, path_, hwidTranslator);
    for (auto &info : infos)
      info_.emplace(std::move(info));

    auto capabilities = provider->provideCapabilities(vendor_, index_, path_);
    for (auto &capability : capabilities)
      capabilities_.emplace(std::move(capability));
  }
}
