// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "igpuinfo.h"
#include "vendor.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

class GPUInfo final : public IGPUInfo
{
 public:
  GPUInfo(Vendor vendor, int index, IGPUInfo::Path &&path) noexcept;

  Vendor vendor() const override;
  int index() const override;
  IGPUInfo::Path const &path() const override;
  std::vector<std::string> keys() const override;
  std::string info(std::string_view key) const override;
  bool hasCapability(std::string_view name) const override;
  void initialize(
      std::vector<std::unique_ptr<IGPUInfo::IProvider>> const &infoProviders,
      IHWIDTranslator const &hwidTranslator) override;

 private:
  Vendor const vendor_;
  int const index_;
  IGPUInfo::Path const path_;
  std::unordered_map<std::string, std::string> info_;
  std::unordered_set<std::string> capabilities_;
};
