// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "item.h"
#include "profilepart.h"
#include <string>

class ISysComponentProfilePart : public ProfilePart
{
 public:
  virtual bool belongsTo(Item const &i) const = 0;
  virtual std::string const &key() const = 0;
};
