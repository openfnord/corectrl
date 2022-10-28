// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/icontrol.h"
#include "trompeloeil.hpp"

class ControlMock : public IControl
{
 public:
  MAKE_MOCK0(init, void(), override);
  MAKE_MOCK1(preInit, void(ICommandQueue &), override);
  MAKE_MOCK1(postInit, void(ICommandQueue &), override);
  MAKE_CONST_MOCK0(active, bool(), override);
  MAKE_MOCK1(activate, void(bool), override);
  MAKE_MOCK0(cleanOnce, void(), override);
  MAKE_MOCK1(clean, void(ICommandQueue &), override);
  MAKE_MOCK1(sync, void(ICommandQueue &), override);
  MAKE_CONST_MOCK0(ID, std::string const &(), override);
  MAKE_MOCK1(importWith, void(Importable::Importer &), override);
  MAKE_CONST_MOCK1(exportWith, void(Exportable::Exporter &), override);
};
