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
#include <unordered_map>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PMPowerProfile : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_POWER_PROFILE"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMPowerProfileMode() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMPowerProfileMode(std::string const &mode) = 0;
    virtual void
    takePMPowerProfileModes(std::vector<std::string> const &modes) = 0;
  };

  PMPowerProfile(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
                 std::unique_ptr<IDataSource<std::vector<std::string>>>
                     &&powerProfileDataSource,
                 std::vector<std::pair<std::string, int>> const &modes) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  void mode(std::string const &mode);
  std::string const &mode() const;
  std::vector<std::string> const &modes() const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const powerProfileDataSource_;

  int currentModeIndex_;
  int defaultModeIndex_;
  std::vector<std::string> modes_;
  std::unordered_map<int, std::string> indexMode_;

  std::string dataSourceEntry_;
  std::vector<std::string> dataSourceLines_;
};

} // namespace AMD
