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
