// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodexmlparser.h"

namespace AMD {

class PMPowerStateModeXMLParser final : public ControlModeXMLParser
{
 public:
  PMPowerStateModeXMLParser() noexcept;

 protected:
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  static bool const registered_;
};

} // namespace AMD
