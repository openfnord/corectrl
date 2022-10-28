// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "../icpuinfo.h"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

class CPUInfoProcCpuInfo final : public ICPUInfo::IProvider
{
 public:
  CPUInfoProcCpuInfo(std::unique_ptr<IDataSource<std::vector<std::string>>>
                         &&dataSource) noexcept;

  std::vector<std::pair<std::string, std::string>> provideInfo(
      int socketId,
      std::vector<ICPUInfo::ExecutionUnit> const &executionUnits) override;

  std::vector<std::string> provideCapabilities(
      int socketId,
      std::vector<ICPUInfo::ExecutionUnit> const &executionUnits) override;

 private:
  void addInfo(std::string_view target, std::string_view key, int cpuId,
               std::vector<std::pair<std::string, std::string>> &info,
               std::vector<std::string> const &lines) const;

  std::unique_ptr<IDataSource<std::vector<std::string>>> const dataSource_;
  static bool registered_;
};
