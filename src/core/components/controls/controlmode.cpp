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
#include "controlmode.h"

#include <algorithm>

ControlMode::ControlMode(std::string_view id,
                         std::vector<std::unique_ptr<IControl>> &&controls,
                         bool active) noexcept
: Control(active, true)
, id_(id)
, controls_(std::move(controls))
{
}

void ControlMode::preInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->preInit(ctlCmds);
}

void ControlMode::init()
{
  // Set the first active control as the active mode
  // or the first one when no active controls are found.

  bool activeFound{false};
  for (auto &control : controls_) {
    control->init();

    if (!activeFound && control->active()) {
      mode(control->ID());
      activeFound = true;
    }
    else if (activeFound && control->active())
      control->activate(false);
  }

  if (!activeFound && !controls_.empty()) {
    auto &control = controls_.front();
    control->activate(true);
    mode(control->ID());
  }
}

std::string const &ControlMode::ID() const
{
  return id_;
}

void ControlMode::importControl(IControl::Importer &i)
{
  auto &pModeImporter = dynamic_cast<ControlMode::Importer &>(i);

  // import mode when is a known mode
  auto &newMode = pModeImporter.provideMode();
  auto iter = std::find_if(
      controls_.cbegin(), controls_.cend(),
      [&](auto &control) { return newMode == control->ID(); });
  if (iter != controls_.cend())
    mode(newMode);

  for (auto &control : controls_)
    control->importWith(i);
}

void ControlMode::exportControl(IControl::Exporter &e) const
{
  std::vector<std::string> modes;
  for (auto &control : controls_) {
    modes.emplace_back(control->ID());
    control->exportWith(e);
  }

  auto &pModeExporter = dynamic_cast<ControlMode::Exporter &>(e);
  pModeExporter.takeModes(modes);
  pModeExporter.takeMode(mode());
}

void ControlMode::cleanControl(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->clean(ctlCmds);
}

void ControlMode::syncControl(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->sync(ctlCmds);
}

std::string const &ControlMode::mode() const
{
  return mode_;
}

void ControlMode::mode(std::string const &mode)
{
  mode_ = mode;
}
