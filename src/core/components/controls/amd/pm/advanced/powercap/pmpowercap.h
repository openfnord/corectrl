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
#include "units/units.h"
#include <string_view>

template<typename...>
class IDataSource;

namespace AMD {

class PMPowerCap : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_POWERCAP"};

  class Importer : public IControl::Importer
  {
   public:
    virtual units::power::watt_t providePMPowerCapValue() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMPowerCapValue(units::power::watt_t value) = 0;
    virtual void takePMPowerCapRange(units::power::watt_t min,
                                     units::power::watt_t max) = 0;
  };

  PMPowerCap(std::unique_ptr<IDataSource<unsigned long>> &&powerCapDataSource,
             units::power::watt_t min, units::power::watt_t max) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  units::power::microwatt_t value() const;
  void value(units::power::microwatt_t value);

  units::power::microwatt_t min() const;
  units::power::microwatt_t max() const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<unsigned long>> const powerCapDataSource_;
  unsigned long powerCapPreInitValue_;

  units::power::microwatt_t min_;
  units::power::microwatt_t const max_;
  units::power::microwatt_t value_;
};

} // namespace AMD
