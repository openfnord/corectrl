// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpuinfo.h"

#include <algorithm>
#include <iterator>
#include <utility>

CPUInfo::CPUInfo(int socketId,
                 std::vector<ICPUInfo::ExecutionUnit> &&executionUnits) noexcept
: socketId_(socketId)
, executionUnits_(std::move(executionUnits))
{
}

int CPUInfo::socketId() const
{
  return socketId_;
}

std::vector<ICPUInfo::ExecutionUnit> const &CPUInfo::executionUnits() const
{
  return executionUnits_;
}

void CPUInfo::addExecutionUnit(ICPUInfo::ExecutionUnit &&unit)
{
  executionUnits_.emplace_back(std::move(unit));
}

std::vector<std::string> CPUInfo::keys() const
{
  std::vector<std::string> keys;
  keys.reserve(info_.size());

  auto const keySelector = [](auto &pair) { return pair.first; };
  std::transform(info_.cbegin(), info_.cend(), std::back_inserter(keys),
                 keySelector);

  return keys;
}

std::string CPUInfo::info(std::string_view key) const
{
  auto const dataIt = info_.find(std::string(key));
  if (dataIt != info_.cend())
    return dataIt->second;

  return {};
}

bool CPUInfo::hasCapability(std::string_view name) const
{
  return capabilities_.count(std::string(name)) > 0;
}

void CPUInfo::initialize(
    std::vector<std::unique_ptr<ICPUInfo::IProvider>> const &providers)
{
  for (auto &provider : providers) {
    auto infos = provider->provideInfo(socketId_, executionUnits_);
    for (auto &info : infos)
      info_.emplace(std::move(info));

    auto capabilities = provider->provideCapabilities(socketId_, executionUnits_);
    for (auto &capability : capabilities)
      capabilities_.emplace(std::move(capability));
  }
}
