// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/isyscomponent.h"
#include <string_view>

class IGPUInfo;
class ISensor;

class IGPU : public ISysComponent
{
 public:
  static constexpr std::string_view ItemID{"GPU"};

  class Exporter : public ISysComponent::Exporter
  {
   public:
    virtual void takeInfo(IGPUInfo const &info) = 0;
    virtual void takeSensor(ISensor const &sensor) = 0;
  };

  virtual IGPUInfo const &info() const = 0;

  virtual ~IGPU() = default;
};
