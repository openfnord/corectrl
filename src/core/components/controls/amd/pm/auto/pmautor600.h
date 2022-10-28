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

class PMAutoR600 : public AMD::PMAuto
{
  static constexpr std::string_view PerfLevel{"auto"};

 public:
  PMAutoR600(
      std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept;

 protected:
  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

 private:
  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::string perfLevelEntry_;
};

} // namespace AMD
