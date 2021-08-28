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
#include "pmvoltoffsetprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>

class AMD::PMVoltOffsetProfilePart::Initializer final
: public AMD::PMVoltOffset::Exporter
{
 public:
  Initializer(AMD::PMVoltOffsetProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMVoltOffsetValue(units::voltage::millivolt_t value) override;
  void takePMVoltOffsetRange(units::voltage::millivolt_t min,
                             units::voltage::millivolt_t max) override;

 private:
  AMD::PMVoltOffsetProfilePart &outer_;
};

void AMD::PMVoltOffsetProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMVoltOffsetProfilePart::Initializer::takePMVoltOffsetValue(
    units::voltage::millivolt_t value)
{
  outer_.value_ = value;
}

void AMD::PMVoltOffsetProfilePart::Initializer::takePMVoltOffsetRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  outer_.range_ = std::make_pair(min, max);
}

AMD::PMVoltOffsetProfilePart::PMVoltOffsetProfilePart() noexcept
: id_(AMD::PMVoltOffset::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMVoltOffsetProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMVoltOffsetProfilePart::initializer()
{
  return std::make_unique<AMD::PMVoltOffsetProfilePart::Initializer>(*this);
}

std::string const &AMD::PMVoltOffsetProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMVoltOffsetProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMVoltOffsetProfilePart::provideActive() const
{
  return active();
}

units::voltage::millivolt_t
AMD::PMVoltOffsetProfilePart::providePMVoltOffsetValue() const
{
  return value_;
}

void AMD::PMVoltOffsetProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmPowerCapProfilePartImporter =
      dynamic_cast<AMD::PMVoltOffsetProfilePart::Importer &>(i);
  value(pmPowerCapProfilePartImporter.providePMVoltOffsetValue());
}

void AMD::PMVoltOffsetProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmPowerCapProfilePartExporter =
      dynamic_cast<AMD::PMVoltOffsetProfilePart::Exporter &>(e);
  pmPowerCapProfilePartExporter.takePMVoltOffsetValue(value_);
}

std::unique_ptr<IProfilePart> AMD::PMVoltOffsetProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMVoltOffsetProfilePart>();
  clone->range_ = range_;
  clone->value_ = value_;

  return std::move(clone);
}

void AMD::PMVoltOffsetProfilePart::value(units::voltage::millivolt_t value)
{
  value_ = std::clamp(value, range_.first, range_.second);
}

bool const AMD::PMVoltOffsetProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMVoltOffset::ItemID, []() {
      return std::make_unique<AMD::PMVoltOffsetProfilePart>();
    });
