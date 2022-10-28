// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <string>
#include <string_view>
#include <vector>

namespace AMD {

class PMFixed : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FIXED"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMFixedMode() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFixedMode(std::string const &mode) = 0;
    virtual void takePMFixedModes(std::vector<std::string> const &modes) = 0;
  };

  PMFixed(std::string_view mode) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  std::string const &mode() const;
  void mode(std::string const &mode);

  virtual std::vector<std::string> const &modes() const = 0;

 private:
  std::string const id_;
  std::string mode_;
};

} // namespace AMD
