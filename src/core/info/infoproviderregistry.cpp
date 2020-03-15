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
