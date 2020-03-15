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
#include "pmautoprofilepart.h"

#include "core/profilepartprovider.h"
#include <memory>

class AMD::PMAutoProfilePart::Initializer final : public AMD::PMAuto::Exporter
{
 public:
  Initializer(AMD::PMAutoProfilePart &outer) noexcept
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
  AMD::PMAutoProfilePart &outer_;
};

void AMD::PMAutoProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

AMD::PMAutoProfilePart::PMAutoProfilePart() noexcept
: id_(AMD::PMAuto::ItemID)
{
}

std::string const &AMD::PMAutoProfilePart::ID() const
{
  return id_;
}

std::unique_ptr<Exportable::Exporter>
AMD::PMAutoProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMAutoProfilePart::initializer()
{
  return std::make_unique<AMD::PMAutoProfilePart::Initializer>(*this);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMAutoProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMAutoProfilePart::provideActive() const
{
  return active();
}

void AMD::PMAutoProfilePart::importProfilePart(IProfilePart::Importer &)
{
}

void AMD::PMAutoProfilePart::exportProfilePart(IProfilePart::Exporter &) const
{
}

std::unique_ptr<IProfilePart> AMD::PMAutoProfilePart::cloneProfilePart() const
{
  return std::make_unique<AMD::PMAutoProfilePart>();
}

bool const AMD::PMAutoProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMAuto::ItemID, []() {
      return std::make_unique<AMD::PMAutoProfilePart>();
    });
