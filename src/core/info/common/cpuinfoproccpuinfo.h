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
