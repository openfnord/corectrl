// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/icpucontrolprovider.h"
#include <memory>
#include <string>
#include <vector>

template<typename...>
class IDataSource;

class CPUFreqProvider final : public ICPUControlProvider::IProvider
{
 public:
  std::vector<std::unique_ptr<IControl>>
  provideCPUControls(ICPUInfo const &cpuInfo,
                     ISWInfo const &swInfo) const override;

 private:
  std::vector<std::string> availableGovernors(ICPUInfo const &cpuInfo) const;
  std::string defatultGovernor(ICPUInfo const &cpuInfo,
                               std::vector<std::string> const &governors) const;
  std::vector<std::unique_ptr<IDataSource<std::string>>>
  createScalingGovernorDataSources(ICPUInfo const &cpuInfo) const;

  static bool const registered_;
};
