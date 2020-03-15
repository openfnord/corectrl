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
#include "controlgroup.h"

#include <utility>

ControlGroup::ControlGroup(std::string_view id,
                           std::vector<std::unique_ptr<IControl>> &&controls,
                           bool active) noexcept
: Control(active, true)
, id_(id)
, controls_(std::move(controls))
{
}

void ControlGroup::preInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->preInit(ctlCmds);
}

void ControlGroup::postInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->postInit(ctlCmds);
}

void ControlGroup::init()
{
  if (!controls_.empty()) {
    for (auto &control : controls_) {
      control->init();

      // activate inactive controls
      if (!control->active())
        control->activate(true);
    }
  }
}

std::string const &ControlGroup::ID() const
{
  return id_;
}

void ControlGroup::importControl(IControl::Importer &i)
{
  for (auto &control : controls_) {
    control->importWith(i);

    // ensure that all controls are active
    control->activate(true);
  }
}

void ControlGroup::exportControl(IControl::Exporter &e) const
{
  for (auto &control : controls_)
    control->exportWith(e);
}

void ControlGroup::cleanControl(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->clean(ctlCmds);
}

void ControlGroup::syncControl(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->sync(ctlCmds);
}
