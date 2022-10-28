// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "control.h"

Control::Control(bool active, bool forceClean) noexcept
: active_(active)
, forceClean_(forceClean)
{
}

bool Control::active() const
{
  return active_;
}

void Control::activate(bool active)
{
  if (active_ && !active)
    dirty(true);

  active_ = active;
}

void Control::cleanOnce()
{
  dirty(true);
}

void Control::importWith(Importable::Importer &i)
{
  auto importer = i.provideImporter(*this);
  if (importer.has_value()) {
    auto &controlImporter = dynamic_cast<IControl::Importer &>(importer->get());

    activate(controlImporter.provideActive());
    importControl(controlImporter);
  }
}

void Control::exportWith(Exportable::Exporter &e) const
{
  auto exporter = e.provideExporter(*this);
  if (exporter.has_value()) {
    auto &controlExporter = dynamic_cast<IControl::Exporter &>(exporter->get());

    controlExporter.takeActive(active());
    exportControl(controlExporter);
  }
}

void Control::clean(ICommandQueue &ctlCmds)
{
  if (forceClean_ || dirty()) {
    cleanControl(ctlCmds);
    dirty(false);
  }
}

void Control::sync(ICommandQueue &ctlCmds)
{
  if (active())
    syncControl(ctlCmds);
}

bool Control::dirty() const
{
  return dirty_;
}

void Control::dirty(bool isDirty)
{
  dirty_ = isDirty;
}
