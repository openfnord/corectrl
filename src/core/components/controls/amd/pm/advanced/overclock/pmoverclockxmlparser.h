// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroupxmlparser.h"

namespace AMD {

class PMOverclockXMLParser final : public ControlGroupXMLParser
{
 public:
  PMOverclockXMLParser() noexcept;

 private:
  static bool const registered_;
};

} // namespace AMD
