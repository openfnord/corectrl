// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilepart.h"

#include "iprofilepartprovider.h"
#include <utility>

bool ProfilePart::active() const
{
  return active_;
}

void ProfilePart::activate(bool active)
{
  active_ = active;
}

std::unique_ptr<IProfilePart> ProfilePart::clone() const
{
  auto clone = cloneProfilePart();
  clone->activate(active());

  return clone;
}

void ProfilePart::importWith(Importable::Importer &i)
{
  auto importer = i.provideImporter(*this);
  if (importer.has_value()) {
    auto &profilePartImporter =
        dynamic_cast<ProfilePart::Importer &>(importer->get());

    activate(profilePartImporter.provideActive());
    importProfilePart(profilePartImporter);
  }
}

void ProfilePart::exportWith(Exportable::Exporter &e) const
{
  auto exporter = e.provideExporter(*this);
  if (exporter.has_value()) {
    auto &profilePartExporter =
        dynamic_cast<ProfilePart::Exporter &>(exporter->get());

    profilePartExporter.takeActive(active());
    exportProfilePart(profilePartExporter);
  }
}

ProfilePart::Factory::Factory(IProfilePartProvider const &profilePartProvider) noexcept
: profilePartProvider_(profilePartProvider)
{
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ProfilePart::Factory::factory(std::string const &componentID)
{
  auto profilePart = createPart(componentID);
  if (profilePart != nullptr) {
    auto factory = profilePart->factory(profilePartProvider_);
    takeProfilePart(std::move(profilePart));

    if (factory != nullptr) {
      factories_.emplace_back(std::move(factory));
      return *factories_.back();
    }
  }

  return {};
}

std::unique_ptr<IProfilePart>
ProfilePart::Factory::createPart(std::string const &componentID) const
{
  auto &partProviders = profilePartProvider_.profilePartProviders();
  auto const providerIt = partProviders.find(componentID);
  if (providerIt != partProviders.cend())
    return providerIt->second();

  return nullptr;
}
