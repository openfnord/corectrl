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
#pragma once

#include "icontrol.h"

class Control : public IControl
{
 public:
  Control(bool active = true, bool forceClean = false) noexcept;

  bool active() const final override;
  void activate(bool active) final override;

  void importWith(Importable::Importer &i) final override;
  void exportWith(Exportable::Exporter &e) const final override;

  void clean(ICommandQueue &ctlCmds) final override;
  void sync(ICommandQueue &ctlCmds) final override;

 protected:
  virtual void importControl(IControl::Importer &i) = 0;
  virtual void exportControl(IControl::Exporter &e) const = 0;

  virtual void cleanControl(ICommandQueue &ctlCmds) = 0;
  virtual void syncControl(ICommandQueue &ctlCmds) = 0;

 private:
  bool dirty() const;
  void dirty(bool isDirty);

  bool active_;
  bool forceClean_;
  bool dirty_{false};
};
