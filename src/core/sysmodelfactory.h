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

#include "components/controls/control.h"
#include "info/icpuinfo.h"
#include "info/igpuinfo.h"
#include "info/vendor.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class ICPU;
class IGPU;
class ISWInfo;
class ISysModel;
class ISysExplorer;
class IHWIDTranslator;
class ICPUControlProvider;
class ICPUSensorProvider;
class IGPUControlProvider;
class IGPUSensorProvider;

class SysModelFactory final
{
 public:
  SysModelFactory(
      std::unique_ptr<ISWInfo> &&swInfo,
      std::unique_ptr<ISysExplorer> &&sysExplorer,
      std::unique_ptr<IHWIDTranslator> &&hwidTranslator,
      std::unique_ptr<ICPUControlProvider> &&cpuControlProvider,
      std::unique_ptr<ICPUSensorProvider> &&cpuSensorProvider,
      std::unique_ptr<IGPUControlProvider> &&gpuControlProvider,
      std::unique_ptr<IGPUSensorProvider> &&gpuSensorProvider,
      ICPUInfo::IProviderRegistry const &cpuInfoProviderRegistry,
      IGPUInfo::IProviderRegistry const &gpuInfoProviderRegistry) noexcept;

  std::unique_ptr<ISysModel> build() const;

 private:
  std::vector<std::unique_ptr<ICPUInfo>> createCPUInfo() const;
  std::unique_ptr<ICPU> createCPU(std::unique_ptr<ICPUInfo> &&cpuInfo,
                                  ISWInfo const &swInfo) const;

  std::vector<std::unique_ptr<IGPUInfo>> createGPUInfo() const;
  std::unique_ptr<IGPU> createGPU(std::unique_ptr<IGPUInfo> &&gpuInfo,
                                  ISWInfo const &swInfo) const;

  std::vector<std::unique_ptr<ICPUInfo>> parseCPUInfo() const;
  int computeGPUIndex(std::string const &deviceRenderDName) const;
  Vendor parseVendor(std::filesystem::path const &vendorPath) const;

  std::shared_ptr<ISWInfo> const swInfo_;
  std::unique_ptr<ISysExplorer> const sysExplorer_;
  std::unique_ptr<IHWIDTranslator> const hwidTranslator_;
  std::unique_ptr<ICPUControlProvider> const cpuControlProvider_;
  std::unique_ptr<ICPUSensorProvider> const cpuSensorProvider_;
  std::unique_ptr<IGPUControlProvider> const gpuControlProvider_;
  std::unique_ptr<IGPUSensorProvider> const gpuSensorProvider_;
  ICPUInfo::IProviderRegistry const &cpuInfoProviderRegistry_;
  IGPUInfo::IProviderRegistry const &gpuInfoProviderRegistry_;
};
