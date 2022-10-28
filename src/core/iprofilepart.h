// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "exportable.h"
#include "importable.h"
#include "item.h"
#include <memory>

class IProfilePartProvider;

class IProfilePart
: public Item
, public Importable
, public Exportable
{
 public:
  class Importer : public Importable::Importer
  {
   public:
    virtual bool provideActive() const = 0;
  };

  class Exporter : public Exportable::Exporter
  {
   public:
    virtual void takeActive(bool active) = 0;
  };

  virtual std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) = 0;

  virtual std::unique_ptr<Exportable::Exporter> initializer() = 0;

  virtual bool active() const = 0;
  virtual void activate(bool active) = 0;

  virtual std::unique_ptr<IProfilePart> clone() const = 0;

  virtual ~IProfilePart() = default;
};
