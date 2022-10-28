// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "units/units.h"

class IHelperControl
{
 public:
  virtual units::time::millisecond_t minExitTimeout() const = 0;

  virtual void init(units::time::millisecond_t autoExitTimeout) = 0;
  virtual void stop() = 0;

  virtual ~IHelperControl() = default;
};
