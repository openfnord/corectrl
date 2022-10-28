// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmode.h"
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace AMD {

class FanMode : public ControlMode
{
 public:
  static constexpr std::string_view ItemID{"AMD_FAN_MODE"};

  FanMode(std::vector<std::unique_ptr<IControl>> &&controls) noexcept
  : ControlMode(FanMode::ItemID, std::move(controls), true)
  {
  }
};

} // namespace AMD
