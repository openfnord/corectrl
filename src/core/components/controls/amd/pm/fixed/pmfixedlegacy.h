//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
