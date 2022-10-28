// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "exportable.h"
#include <memory>
#include <string>
#include <vector>

class IProfile;

class IProfileParser
{
 public:
  virtual std::string const &format() = 0;

  virtual std::unique_ptr<Exportable::Exporter> initializer() = 0;

  virtual bool load(std::vector<char> const &data, IProfile &profile) = 0;
  virtual bool save(std::vector<char> &data, IProfile const &profile) = 0;

  virtual ~IProfileParser() = default;
};
