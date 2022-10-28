// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodeqmlitem.h"

namespace AMD {

class PMPerfModeQMLItem : public ControlModeQMLItem
{
 public:
  explicit PMPerfModeQMLItem() noexcept;

 private:
  static bool register_();
  static bool const registered_;
};

} // namespace AMD
