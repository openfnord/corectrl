// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "pmauto.h"
#include <memory>
#include <string>
#include <string_view>

template<typename...>
class IDataSource;

namespace AMD {

class PMAutoLegacy : public AMD::PMAuto
{
  static constexpr std::string_view Method{"profile"};
  static constexpr std::string_view Profile{"auto"};

 public:
  PMAutoLegacy(
      std::unique_ptr<IDataSource<std::string>> &&powerMethodDataSource,
      std::unique_ptr<IDataSource<std::string>> &&powerProfileDataSource) noexcept;

 protected:
  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

 private:
  std::unique_ptr<IDataSource<std::string>> const powerMethodDataSource_;
  std::unique_ptr<IDataSource<std::string>> const powerProfileDataSource_;
  std::string powerMethodEntry_;
  std::string powerProfileEntry_;
};

} // namespace AMD
