// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofile.h"

class ISysModelUI
: public IProfile::Importer
, public IProfile::Exporter
{
 public:
  virtual ~ISysModelUI() = default;
};
