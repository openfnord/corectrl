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

#include "core/components/controls/control.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PMPowerState : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_POWERSTATE"};

  struct State
  {
    static constexpr std::string_view Battery{"battery"};
    static constexpr std::string_view Balanced{"balanced"};
    static constexpr std::string_view Performance{"performance"};
  };

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMPowerStateMode() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMPowerStateMode(std::string const &mode) = 0;
    virtual void takePMPowerStateModes(std::vector<std::string> const &modes) = 0;
  };

  PMPowerState(std::unique_ptr<IDataSource<std::string>>
                   &&powerDpmStateDataSource) noexcept;

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

  std::vector<std::string> const &modes() const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::string>> const powerDpmStateDataSource_;
  std::string powerDpmStateEntry_;
  std::string mode_;

  static std::vector<std::string> const modes_;
};

} // namespace AMD
