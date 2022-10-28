// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/iprofilepart.h"
#include "core/isyscomponentprofilepart.h"

class ICPUProfilePart : public ISysComponentProfilePart
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual int provideSocketId() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeSocketId(int id) = 0;
  };

  virtual ~ICPUProfilePart() = default;
};
