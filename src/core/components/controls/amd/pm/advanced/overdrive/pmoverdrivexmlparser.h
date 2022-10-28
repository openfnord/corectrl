// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroupxmlparser.h"

namespace AMD {

class PMOverdriveXMLParser final : public ControlGroupXMLParser
{
 public:
  PMOverdriveXMLParser() noexcept;

 protected:
  void loadPartFrom(pugi::xml_node const &parentNode) final override;

 private:
  static bool const registered_;
};

} // namespace AMD
