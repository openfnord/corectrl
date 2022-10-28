// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <string>

class IProfilePart;

class IProfilePartView
{
 public:
  virtual std::string const &profile() const = 0;
  virtual std::shared_ptr<IProfilePart> const &part() const = 0;

  virtual ~IProfilePartView() = default;
};
