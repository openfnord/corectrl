// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroup.h"
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace AMD {

class PMOverclock : public ControlGroup
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_OVERCLOCK"};

  PMOverclock(std::vector<std::unique_ptr<IControl>> &&controls) noexcept
  : ControlGroup(AMD::PMOverclock::ItemID, std::move(controls), true)
  {
  }
};

} // namespace AMD
