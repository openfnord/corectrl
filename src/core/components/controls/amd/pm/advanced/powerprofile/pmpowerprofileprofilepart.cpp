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
#include "pmpowerprofileprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>
#include <utility>

class AMD::PMPowerProfileProfilePart::Initializer final
: public PMPowerProfile::Exporter
{
 public:
  Initializer(AMD::PMPowerProfileProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMPowerProfileMode(std::string const &mode) override;
  void takePMPowerProfileModes(std::vector<std::string> const &modes) override;

 private:
  AMD::PMPowerProfileProfilePart &outer_;
};

void AMD::PMPowerProfileProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMPowerProfileProfilePart::Initializer::takePMPowerProfileMode(
    std::string const &mode)
{
  outer_.mode_ = mode;
}

void AMD::PMPowerProfileProfilePart::Initializer::takePMPowerProfileModes(
    std::vector<std::string> const &modes)
{
  outer_.modes_ = modes;
}

AMD::PMPowerProfileProfilePart::PMPowerProfileProfilePart() noexcept
: id_(AMD::PMPowerProfile::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMPowerProfileProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerProfileProfilePart::initializer()
{
  return std::make_unique<AMD::PMPowerProfileProfilePart::Initializer>(*this);
}

std::string const &AMD::PMPowerProfileProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerProfileProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMPowerProfileProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMPowerProfileProfilePart::providePMPowerProfileMode() const
{
  return mode_;
}

void AMD::PMPowerProfileProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmPowerProfileImporter =
      dynamic_cast<AMD::PMPowerProfileProfilePart::Importer &>(i);
  mode(pmPowerProfileImporter.providePMPowerProfileMode());
}

void AMD::PMPowerProfileProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmPowerProfileExporter =
      dynamic_cast<AMD::PMPowerProfileProfilePart::Exporter &>(e);
  pmPowerProfileExporter.takePMPowerProfileMode(mode_);
}

std::unique_ptr<IProfilePart> AMD::PMPowerProfileProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMPowerProfileProfilePart>();
  clone->modes_ = modes_;
  clone->mode_ = mode_;

  return std::move(clone);
}

void AMD::PMPowerProfileProfilePart::mode(std::string const &mode)
{
  auto iter = std::find_if(
      modes_.cbegin(), modes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != modes_.cend())
    mode_ = mode;
}

bool const AMD::PMPowerProfileProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMPowerProfile::ItemID, []() {
      return std::make_unique<AMD::PMPowerProfileProfilePart>();
    });
