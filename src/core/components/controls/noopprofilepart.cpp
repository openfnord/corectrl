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
#include "noopprofilepart.h"

#include "core/profilepartprovider.h"
#include <memory>

class NoopProfilePart::Initializer final : public Noop::Exporter
{
 public:
  Initializer(NoopProfilePart &outer) noexcept
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
  NoopProfilePart &outer_;
};

void NoopProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

NoopProfilePart::NoopProfilePart() noexcept
: id_(Noop::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
NoopProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> NoopProfilePart::initializer()
{
  return std::make_unique<NoopProfilePart::Initializer>(*this);
}

std::string const &NoopProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
NoopProfilePart::provideImporter(Item const &)
{
  return {};
}

bool NoopProfilePart::provideActive() const
{
  return active();
}

void NoopProfilePart::importProfilePart(IProfilePart::Importer &)
{
}

void NoopProfilePart::exportProfilePart(IProfilePart::Exporter &) const
{
}

std::unique_ptr<IProfilePart> NoopProfilePart::cloneProfilePart() const
{
  return std::make_unique<NoopProfilePart>();
}

bool const NoopProfilePart::registered_ = ProfilePartProvider::registerProvider(
    Noop::ItemID, []() { return std::make_unique<NoopProfilePart>(); });
