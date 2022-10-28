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

class PMFixedLegacy : public PMFixed
{
  static constexpr std::string_view Method{"profile"};
  struct Profile
  {
    static constexpr std::string_view low{"low"};
    static constexpr std::string_view mid{"mid"};
    static constexpr std::string_view high{"high"};
    static constexpr std::string_view clean{"auto"};
  };

 public:
  PMFixedLegacy(
      std::unique_ptr<IDataSource<std::string>> &&powerMethodDataSource,
      std::unique_ptr<IDataSource<std::string>> &&powerProfileDataSource) noexcept;

 protected:
  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  std::vector<std::string> const &modes() const final override;

 private:
  std::unique_ptr<IDataSource<std::string>> const powerMethodDataSource_;
  std::unique_ptr<IDataSource<std::string>> const powerProfileDataSource_;
  std::string powerMethodEntry_;
  std::string powerProfileEntry_;

  static std::vector<std::string> const modes_;
};

} // namespace AMD
