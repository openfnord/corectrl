// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

class Item
{
 public:
  virtual std::string const &ID() const = 0;

  /// Returns the instance ID.
  ///
  /// When there are multiple instances of the same item, an unique
  /// instance ID will be returned for each item. Otherwise, the
  /// returned instance ID will be the value of the item ID.
  virtual std::string const &instanceID() const
  {
    return ID();
  }

  virtual ~Item() = default;
};
