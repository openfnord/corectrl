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
#include "pmdynamicfreqprofilepart.h"

#include "core/profilepartprovider.h"
#include <memory>

class AMD::PMDynamicFreqProfilePart::Initializer final
: public PMDynamicFreq::Exporter
{
 public:
  Initializer(AMD::PMDynamicFreqProfilePart &outer) noexcept
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
  AMD::PMDynamicFreqProfilePart &outer_;
};

void AMD::PMDynamicFreqProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

AMD::PMDynamicFreqProfilePart::PMDynamicFreqProfilePart() noexcept
: id_(AMD::PMDynamicFreq::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMDynamicFreqProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMDynamicFreqProfilePart::initializer()
{
  return std::make_unique<AMD::PMDynamicFreqProfilePart::Initializer>(*this);
}

std::string const &AMD::PMDynamicFreqProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMDynamicFreqProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMDynamicFreqProfilePart::provideActive() const
{
  return active();
}

void AMD::PMDynamicFreqProfilePart::importProfilePart(IProfilePart::Importer &)
{
}

void AMD::PMDynamicFreqProfilePart::exportProfilePart(IProfilePart::Exporter &) const
{
}

std::unique_ptr<IProfilePart> AMD::PMDynamicFreqProfilePart::cloneProfilePart() const
{
  return std::make_unique<AMD::PMDynamicFreqProfilePart>();
}

bool const AMD::PMDynamicFreqProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMDynamicFreq::ItemID, []() {
      return std::make_unique<AMD::PMDynamicFreqProfilePart>();
    });
