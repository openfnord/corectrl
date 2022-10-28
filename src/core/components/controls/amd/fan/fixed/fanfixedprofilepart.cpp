// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanfixedprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>
#include <utility>

class AMD::FanFixedProfilePart::Initializer final : public AMD::FanFixed::Exporter
{
 public:
  Initializer(AMD::FanFixedProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takeFanFixedValue(units::concentration::percent_t value) override;
  void takeFanFixedFanStop(bool enabled) override;
  void takeFanFixedFanStartValue(units::concentration::percent_t value) override;

 private:
  AMD::FanFixedProfilePart &outer_;
};

void AMD::FanFixedProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::FanFixedProfilePart::Initializer::takeFanFixedValue(
    units::concentration::percent_t value)
{
  outer_.value_ = value;
}

void AMD::FanFixedProfilePart::Initializer::takeFanFixedFanStop(bool enabled)
{
  outer_.fanStop_ = enabled;
}

void AMD::FanFixedProfilePart::Initializer::takeFanFixedFanStartValue(
    units::concentration::percent_t value)
{
  outer_.fanStartValue_ = value;
}

AMD::FanFixedProfilePart::FanFixedProfilePart() noexcept
: id_(AMD::FanFixed::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::FanFixedProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::FanFixedProfilePart::initializer()
{
  return std::make_unique<AMD::FanFixedProfilePart::Initializer>(*this);
}

std::string const &AMD::FanFixedProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanFixedProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::FanFixedProfilePart::provideActive() const
{
  return active();
}

units::concentration::percent_t AMD::FanFixedProfilePart::provideFanFixedValue() const
{
  return value_;
}

bool AMD::FanFixedProfilePart::provideFanFixedFanStop() const
{
  return fanStop_;
}

units::concentration::percent_t
AMD::FanFixedProfilePart::provideFanFixedFanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanFixedProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmfImporter = dynamic_cast<AMD::FanFixedProfilePart::Importer &>(i);

  value(pmfImporter.provideFanFixedValue());
  fanStop_ = pmfImporter.provideFanFixedFanStop();
  startValue(pmfImporter.provideFanFixedFanStartValue());
}

void AMD::FanFixedProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmfExporter = dynamic_cast<AMD::FanFixedProfilePart::Exporter &>(e);
  pmfExporter.takeFanFixedValue(value_);
  pmfExporter.takeFanFixedFanStop(fanStop_);
  pmfExporter.takeFanFixedFanStartValue(fanStartValue_);
}

std::unique_ptr<IProfilePart> AMD::FanFixedProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::FanFixedProfilePart>();
  clone->value_ = value_;
  clone->fanStop_ = fanStop_;
  clone->fanStartValue_ = fanStartValue_;

  return std::move(clone);
}

void AMD::FanFixedProfilePart::value(units::concentration::percent_t value)
{
  value_ = std::clamp(value, units::concentration::percent_t(0),
                      units::concentration::percent_t(100));
}

void AMD::FanFixedProfilePart::startValue(units::concentration::percent_t value)
{
  fanStartValue_ = std::clamp(value, units::concentration::percent_t(0),
                              units::concentration::percent_t(100));
}

bool const AMD::FanFixedProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::FanFixed::ItemID, []() {
      return std::make_unique<AMD::FanFixedProfilePart>();
    });
