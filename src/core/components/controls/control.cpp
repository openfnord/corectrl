//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
