// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "controlgroup.h"

#include <utility>

ControlGroup::ControlGroup(std::string_view id,
                           std::vector<std::unique_ptr<IControl>> &&controls,
                           bool active) noexcept
: Control(active)
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

void ControlGroup::activate(bool active)
{
  Control::activate(active);

  if (dirty()) {
    // All aggregated controls are always active. Therefore, we must
    // clean them manually as they will never enter in dirty state on
    // their own.
    for (auto &control : controls_)
      control->cleanOnce();
  }
}

void ControlGroup::cleanOnce()
{
  Control::cleanOnce();

  // propagate clean once to aggregated controls
  for (auto &control : controls_)
    control->cleanOnce();
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
