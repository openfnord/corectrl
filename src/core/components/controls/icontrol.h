// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/exportable.h"
#include "core/importable.h"
#include "core/item.h"

class ICommandQueue;

class IControl
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

  virtual void preInit(ICommandQueue &ctlCmds) = 0;
  virtual void postInit(ICommandQueue &ctlCmds) = 0;
  virtual void init() = 0;

  virtual bool active() const = 0;
  virtual void activate(bool active) = 0;

  /// Cleans the control unconditionally on the next clean method
  /// call, independently of its previous active state.
  virtual void cleanOnce() = 0;

  virtual void clean(ICommandQueue &ctlCmds) = 0;
  virtual void sync(ICommandQueue &ctlCmds) = 0;

  virtual ~IControl() = default;
};
