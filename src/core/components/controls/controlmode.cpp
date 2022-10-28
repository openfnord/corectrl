// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "controlmode.h"

#include <algorithm>
#include <utility>

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

void ControlMode::postInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->postInit(ctlCmds);
}

void ControlMode::init()
{
  // Set the first active control as the active mode
  // or the first one when no active controls are found.

  bool activeFound{false};
  for (auto &control : controls_) {
    control->init();

    if (!activeFound && control->active()) {
      mode_ = control->ID();
      activeFound = true;
    }
    else if (activeFound && control->active())
      control->activate(false);
  }

  if (!activeFound && !controls_.empty()) {
    auto &control = controls_.front();
    control->activate(true);
    mode_ = control->ID();
  }
}

std::string const &ControlMode::ID() const
{
  return id_;
}

void ControlMode::importControl(IControl::Importer &i)
{
  auto &pModeImporter = dynamic_cast<ControlMode::Importer &>(i);
  mode(pModeImporter.provideMode());

  for (auto &control : controls_) {
    control->importWith(i);

    // ensure that only the selected mode is active
    control->activate(control->ID() == mode());
  }
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
  auto iter = std::find_if(controls_.cbegin(), controls_.cend(),
                           [&](auto &control) { return mode == control->ID(); });
  if (iter != controls_.cend())
    mode_ = mode;
}
