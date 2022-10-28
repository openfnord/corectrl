// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/iprofilepart.h"
#include "core/isyscomponentprofilepart.h"
#include <optional>
#include <string>

class IGPUProfilePart : public ISysComponentProfilePart
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual int provideIndex() const = 0;
    virtual std::string const &provideDeviceID() const = 0;
    virtual std::string const &provideRevision() const = 0;
    virtual std::optional<std::string> provideUniqueID() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeIndex(int index) = 0;
    virtual void takeDeviceID(std::string const &deviceID) = 0;
    virtual void takeRevision(std::string const &revision) = 0;
    virtual void takeUniqueID(std::optional<std::string> uniqueID) = 0;
  };

  virtual ~IGPUProfilePart() = default;
};
