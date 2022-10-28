// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "info/vendor.h"
#include "isysexplorer.h"
#include <filesystem>
#include <vector>

class SysExplorer final : public ISysExplorer
{
 public:
  SysExplorer(std::vector<Vendor> gpuVendors) noexcept;

  std::vector<std::string> renderers() override;

 private:
  bool checkGPUVendor(std::filesystem::path sysPath) const;

  std::vector<Vendor> gpuVendors_;
};
