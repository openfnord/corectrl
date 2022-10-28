// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodexmlparser.h"

namespace AMD {

class FanModeXMLParser final : public ControlModeXMLParser
{
 public:
  FanModeXMLParser() noexcept;

 private:
  static bool const registered_;
};

} // namespace AMD
