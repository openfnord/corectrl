//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
