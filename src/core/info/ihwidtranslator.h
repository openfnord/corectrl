// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

class IHWIDTranslator
{
 public:
  /// returns vendor name or empty string if not found
  virtual std::string vendor(std::string const &vendorID) const = 0;

  /// returns device name or empty string if not found
  virtual std::string device(std::string const &vendorID,
                             std::string const &deviceID) const = 0;

  /// returns subdevice name or empty string if not found
  virtual std::string subdevice(std::string const &vendorID,
                                std::string const &deviceID,
                                std::string const &subvendorID,
                                std::string const &subdeviceID) const = 0;

  virtual ~IHWIDTranslator() = default;
};
