// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <string>
#include <string_view>

namespace AMD {

class PMAuto : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_AUTO"};

  PMAuto() noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

 private:
  std::string const id_;
};

} // namespace AMD
