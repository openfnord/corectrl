// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowercapprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>

class AMD::PMPowerCapProfilePart::Initializer final
: public AMD::PMPowerCap::Exporter
{
 public:
  Initializer(AMD::PMPowerCapProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMPowerCapValue(units::power::watt_t value) override;
  void takePMPowerCapRange(units::power::watt_t min,
                           units::power::watt_t max) override;

 private:
  AMD::PMPowerCapProfilePart &outer_;
};

void AMD::PMPowerCapProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMPowerCapProfilePart::Initializer::takePMPowerCapValue(
    units::power::watt_t value)
{
  outer_.value_ = value;
}

void AMD::PMPowerCapProfilePart::Initializer::takePMPowerCapRange(
    units::power::watt_t min, units::power::watt_t max)
{
  outer_.range_ = std::make_pair(min, max);
}

AMD::PMPowerCapProfilePart::PMPowerCapProfilePart() noexcept
: id_(AMD::PMPowerCap::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMPowerCapProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerCapProfilePart::initializer()
{
  return std::make_unique<AMD::PMPowerCapProfilePart::Initializer>(*this);
}

std::string const &AMD::PMPowerCapProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerCapProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMPowerCapProfilePart::provideActive() const
{
  return active();
}

units::power::watt_t AMD::PMPowerCapProfilePart::providePMPowerCapValue() const
{
  return value_;
}

void AMD::PMPowerCapProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmPowerCapProfilePartImporter =
      dynamic_cast<AMD::PMPowerCapProfilePart::Importer &>(i);
  value(pmPowerCapProfilePartImporter.providePMPowerCapValue());
}

void AMD::PMPowerCapProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmPowerCapProfilePartExporter =
      dynamic_cast<AMD::PMPowerCapProfilePart::Exporter &>(e);
  pmPowerCapProfilePartExporter.takePMPowerCapValue(value_);
}

std::unique_ptr<IProfilePart> AMD::PMPowerCapProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMPowerCapProfilePart>();
  clone->range_ = range_;
  clone->value_ = value_;

  return std::move(clone);
}

void AMD::PMPowerCapProfilePart::value(units::power::watt_t value)
{
  value_ = std::clamp(value, range_.first, range_.second);
}

bool const AMD::PMPowerCapProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMPowerCap::ItemID, []() {
      return std::make_unique<AMD::PMPowerCapProfilePart>();
    });
