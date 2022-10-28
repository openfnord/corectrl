// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodexmlparser.h"

class CPUFreqModeXMLParser final : public ControlModeXMLParser
{
 public:
  CPUFreqModeXMLParser() noexcept;

 protected:
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  static bool const registered_;
};
