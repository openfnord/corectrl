// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// A control that aggregates other controls,
/// having only one of them active at the same time.
class ControlMode : public Control
{
 public:
  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &provideMode() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takeMode(std::string const &mode) = 0;
    virtual void takeModes(std::vector<std::string> const &modes) = 0;
  };

  ControlMode(std::string_view id,
              std::vector<std::unique_ptr<IControl>> &&controls,
              bool active) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;
  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  std::string const &mode() const;
  void mode(std::string const &mode);

 private:
  std::string const id_;
  std::vector<std::unique_ptr<IControl>> const controls_;
  std::string mode_;
};
