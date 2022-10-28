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

class CPUInfoLsCpu final : public ICPUInfo::IProvider
{
 public:
  struct Keys
  {

    static constexpr std::string_view arch{"arch"};
    static constexpr std::string_view opMode{"opmode"};
    static constexpr std::string_view byteOrder{"byteorder"};
    static constexpr std::string_view virt{"virt"};
    static constexpr std::string_view l1dCache{"l1dcache"};
    static constexpr std::string_view l1iCache{"l1icache"};
    static constexpr std::string_view l2Cache{"l2cache"};
  };

  CPUInfoLsCpu(std::unique_ptr<IDataSource<std::vector<std::string>>>
                   &&dataSource) noexcept;

  std::vector<std::pair<std::string, std::string>> provideInfo(
      int socketId,
      std::vector<ICPUInfo::ExecutionUnit> const &executionUnits) override;

  std::vector<std::string> provideCapabilities(
      int socketId,
      std::vector<ICPUInfo::ExecutionUnit> const &executionUnits) override;

 private:
  std::string extractLineData(std::string const &line) const;
  void addInfo(std::string_view target, std::string_view key,
               std::vector<std::pair<std::string, std::string>> &info,
               std::vector<std::string> const &data) const;

  std::unique_ptr<IDataSource<std::vector<std::string>>> const dataSource_;
  static bool registered_;
};
