// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/isyscomponent.h"
#include <string_view>

class ICPUInfo;
class ISensor;

class ICPU : public ISysComponent
{
 public:
  static constexpr std::string_view ItemID{"CPU"};

  class Exporter : public ISysComponent::Exporter
  {
   public:
    virtual void takeInfo(ICPUInfo const &info) = 0;
    virtual void takeSensor(ISensor const &sensor) = 0;
  };

  virtual ICPUInfo const &info() const = 0;

  virtual ~ICPU() = default;
};
