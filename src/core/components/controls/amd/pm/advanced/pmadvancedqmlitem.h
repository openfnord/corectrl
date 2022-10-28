// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroupqmlitem.h"

namespace AMD {

class PMAdvancedQMLItem : public ControlGroupQMLItem
{
 public:
  explicit PMAdvancedQMLItem() noexcept;

 private:
  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
