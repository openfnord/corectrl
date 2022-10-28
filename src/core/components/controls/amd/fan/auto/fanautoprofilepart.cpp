// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanautoprofilepart.h"

#include "core/profilepartprovider.h"
#include <memory>

class AMD::FanAutoProfilePart::Initializer final : public AMD::FanAuto::Exporter
{
 public:
  Initializer(AMD::FanAutoProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;

 private:
  AMD::FanAutoProfilePart &outer_;
};

void AMD::FanAutoProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

AMD::FanAutoProfilePart::FanAutoProfilePart() noexcept
: id_(AMD::FanAuto::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::FanAutoProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::FanAutoProfilePart::initializer()
{
  return std::make_unique<AMD::FanAutoProfilePart::Initializer>(*this);
}

std::string const &AMD::FanAutoProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanAutoProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::FanAutoProfilePart::provideActive() const
{
  return active();
}

void AMD::FanAutoProfilePart::importProfilePart(IProfilePart::Importer &)
{
}

void AMD::FanAutoProfilePart::exportProfilePart(IProfilePart::Exporter &) const
{
}

std::unique_ptr<IProfilePart> AMD::FanAutoProfilePart::cloneProfilePart() const
{
  return std::make_unique<AMD::FanAutoProfilePart>();
}

bool const AMD::FanAutoProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::FanAuto::ItemID, []() {
      return std::make_unique<AMD::FanAutoProfilePart>();
    });
