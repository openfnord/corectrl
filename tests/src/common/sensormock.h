// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/sensors/isensor.h"
#include "trompeloeil.hpp"

class SensorMock : public ISensor
{
 public:
  MAKE_MOCK0(update, void(), override);
  MAKE_CONST_MOCK0(ID, std::string const &(), override);
  MAKE_CONST_MOCK1(exportWith, void(Exportable::Exporter &), override);
};
