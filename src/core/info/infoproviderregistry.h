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
