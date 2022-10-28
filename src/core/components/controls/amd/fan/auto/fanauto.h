// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include <memory>
#include <string>
#include <string_view>

template<typename...>
class IDataSource;

namespace AMD {

class FanAuto : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_FAN_AUTO"};

  FanAuto(std::unique_ptr<IDataSource<unsigned int>> &&dataSource) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<unsigned int>> const dataSource_;
  unsigned int pwmEnable_;
};

} // namespace AMD
