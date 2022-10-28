// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// A control that aggregates other controls.
class ControlGroup : public Control
{
 public:
  ControlGroup(std::string_view id,
               std::vector<std::unique_ptr<IControl>> &&controls,
               bool active) noexcept;

  void preInit(ICommandQueue &ctlCmds) override;
  void postInit(ICommandQueue &ctlCmds) override;
  void init() final override;
  std::string const &ID() const final override;

  void activate(bool active) final override;

  void cleanOnce() final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) override;
  void syncControl(ICommandQueue &ctlCmds) override;

 private:
  std::string const id_;
  std::vector<std::unique_ptr<IControl>> const controls_;
};
