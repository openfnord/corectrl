// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmode.h"
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace AMD {

class PMPowerStateMode : public ControlMode
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_POWERSTATE_MODE"};

  PMPowerStateMode(std::vector<std::unique_ptr<IControl>> &&controls) noexcept
  : ControlMode(AMD::PMPowerStateMode::ItemID, std::move(controls), true)
  {
  }
};

} // namespace AMD
