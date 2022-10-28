// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/exportable.h"
#include "core/item.h"

class ISensor
: public Item
, public Exportable
{
 public:
  virtual void update() = 0;

  virtual ~ISensor() = default;
};
