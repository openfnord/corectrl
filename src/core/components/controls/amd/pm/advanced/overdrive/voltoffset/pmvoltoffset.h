//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "units/units.h"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

/// Overdrive voltage offset control.
class PMVoltOffset : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_VOLT_OFFSET"};

  class Importer : public IControl::Importer
  {
   public:
    virtual units::voltage::millivolt_t providePMVoltOffsetValue() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMVoltOffsetRange(units::voltage::millivolt_t min,
                                       units::voltage::millivolt_t max) = 0;
    virtual void takePMVoltOffsetValue(units::voltage::millivolt_t value) = 0;
  };

  PMVoltOffset(std::unique_ptr<IDataSource<std::vector<std::string>>>
                   &&ppOdClkVoltDataSource) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) override;
  void syncControl(ICommandQueue &ctlCmds) override;

  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const &
  range() const;

  units::voltage::millivolt_t value() const;
  void value(units::voltage::millivolt_t value);

  std::string ppOdClkVoltCmd(units::voltage::millivolt_t offset) const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;
  std::vector<std::string> ppOdClkVoltLines_;

  units::voltage::millivolt_t preInitOffset_;
  units::voltage::millivolt_t value_;
  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const range_;
};

} // namespace AMD
