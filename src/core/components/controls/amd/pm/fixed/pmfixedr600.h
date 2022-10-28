// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "pmfixed.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PMFixedR600 : public AMD::PMFixed
{
  struct PerfLevel
  {
    static constexpr std::string_view low{"low"};
    static constexpr std::string_view high{"high"};
    static constexpr std::string_view clean{"auto"};
  };

 public:
  PMFixedR600(
      std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept;

 protected:
  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  std::vector<std::string> const &modes() const final override;

 private:
  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::string perfLevelEntry_;

  static std::vector<std::string> const modes_;
};

} // namespace AMD
