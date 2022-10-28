// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmode.h"
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

class CPUFreqMode : public ControlMode
{
 public:
  static constexpr std::string_view ItemID{"CPU_CPUFREQ_MODE"};

  CPUFreqMode(std::vector<std::unique_ptr<IControl>> &&controls) noexcept
  : ControlMode(CPUFreqMode::ItemID, std::move(controls), true)
  {
  }
};
