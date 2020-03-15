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
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CPUInfo final : public ICPUInfo
{
 public:
  CPUInfo(int socketId,
          std::vector<ICPUInfo::ExecutionUnit> &&executionUnits) noexcept;

  int socketId() const override;
  std::vector<ICPUInfo::ExecutionUnit> const &executionUnits() const override;
  std::vector<std::string> keys() const override;
  std::string info(std::string_view key) const override;
  bool hasCapability(std::string_view name) const override;
  void initialize(
      std::vector<std::unique_ptr<ICPUInfo::IProvider>> const &providers) override;

  void addExecutionUnit(ICPUInfo::ExecutionUnit &&unit);

 private:
  int const socketId_;
  std::vector<ICPUInfo::ExecutionUnit> executionUnits_;
  std::unordered_map<std::string, std::string> info_;
  std::unordered_set<std::string> capabilities_;
};
