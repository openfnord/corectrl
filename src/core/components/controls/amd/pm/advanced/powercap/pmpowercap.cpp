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
#include "pmpowercap.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>

AMD::PMPowerCap::PMPowerCap(
    std::unique_ptr<IDataSource<unsigned long>> &&powerCapDataSource,
    units::power::watt_t min, units::power::watt_t max) noexcept
: Control(true)
, id_(AMD::PMPowerCap::ItemID)
, powerCapDataSource_(std::move(powerCapDataSource))
, min_(min)
, max_(max)
, value_(1)
{
  if (min_ == units::power::watt_t(0))
    min_ = units::power::watt_t(1);
}

void AMD::PMPowerCap::preInit(ICommandQueue &ctlCmds)
{
  cleanControl(ctlCmds);
}

void AMD::PMPowerCap::init()
{
  unsigned long powerCapValue;
  if (powerCapDataSource_->read(powerCapValue))
    value_ = units::power::microwatt_t(powerCapValue);
}

std::string const &AMD::PMPowerCap::ID() const
{
  return id_;
}

void AMD::PMPowerCap::importControl(IControl::Importer &i)
{
  auto &pmPowerCapImporter = dynamic_cast<AMD::PMPowerCap::Importer &>(i);
  value(pmPowerCapImporter.providePMPowerCapValue());
}

void AMD::PMPowerCap::exportControl(IControl::Exporter &e) const
{
  auto &pmPowerCapExporter = dynamic_cast<AMD::PMPowerCap::Exporter &>(e);
  pmPowerCapExporter.takePMPowerCapRange(min(), max());
  pmPowerCapExporter.takePMPowerCapValue(value());
}

void AMD::PMPowerCap::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({powerCapDataSource_->source(), "0"});
}

void AMD::PMPowerCap::syncControl(ICommandQueue &ctlCmds)
{
  unsigned long powerCapValue;
  if (powerCapDataSource_->read(powerCapValue)) {
    if (units::power::microwatt_t(powerCapValue) != value())
      ctlCmds.add({powerCapDataSource_->source(),
                   std::to_string(value().to<unsigned long>())});
  }
}

units::power::microwatt_t AMD::PMPowerCap::value() const
{
  return value_;
}

void AMD::PMPowerCap::value(units::power::microwatt_t value)
{
  value_ = std::clamp(value, min(), max());
}

units::power::microwatt_t AMD::PMPowerCap::min() const
{
  return min_;
}

units::power::microwatt_t AMD::PMPowerCap::max() const
{
  return max_;
}
