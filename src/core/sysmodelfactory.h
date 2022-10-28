// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
