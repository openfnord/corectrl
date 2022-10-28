// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodeqmlitem.h"

namespace AMD {

class PMPowerStateModeQMLItem : public ControlModeQMLItem
{
 public:
  explicit PMPowerStateModeQMLItem() noexcept;

 private:
  static bool register_();
  static bool const registered_;
};

} // namespace AMD
