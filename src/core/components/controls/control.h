// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icontrol.h"

class Control : public IControl
{
 public:
  Control(bool active = true, bool forceClean = false) noexcept;

  bool active() const final override;
  void activate(bool active) override;

  void cleanOnce() override;

  void importWith(Importable::Importer &i) final override;
  void exportWith(Exportable::Exporter &e) const final override;

  void clean(ICommandQueue &ctlCmds) final override;
  void sync(ICommandQueue &ctlCmds) final override;

 protected:
  virtual void importControl(IControl::Importer &i) = 0;
  virtual void exportControl(IControl::Exporter &e) const = 0;

  virtual void cleanControl(ICommandQueue &ctlCmds) = 0;
  virtual void syncControl(ICommandQueue &ctlCmds) = 0;

  bool dirty() const;
  void dirty(bool isDirty);

 protected:
  bool active_;
  bool forceClean_;
  bool dirty_{false};
};
