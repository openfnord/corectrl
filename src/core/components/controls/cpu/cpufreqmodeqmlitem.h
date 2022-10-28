// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodeqmlitem.h"

class CPUFreqModeQMLItem : public ControlModeQMLItem
{
 public:
  explicit CPUFreqModeQMLItem() noexcept;

 private:
  static bool register_();
  static bool const registered_;
};
