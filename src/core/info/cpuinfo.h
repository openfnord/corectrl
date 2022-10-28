// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
