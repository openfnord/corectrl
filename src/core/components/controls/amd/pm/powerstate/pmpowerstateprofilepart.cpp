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
#include "pmpowerstateprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>
#include <utility>

class AMD::PMPowerStateProfilePart::Initializer final
: public AMD::PMPowerState::Exporter
{
 public:
  Initializer(AMD::PMPowerStateProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMPowerStateMode(std::string const &mode) override;
  void takePMPowerStateModes(std::vector<std::string> const &modes) override;

 private:
  AMD::PMPowerStateProfilePart &outer_;
};

void AMD::PMPowerStateProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMPowerStateProfilePart::Initializer::takePMPowerStateMode(
    std::string const &mode)
{
  outer_.mode_ = mode;
}

void AMD::PMPowerStateProfilePart::Initializer::takePMPowerStateModes(
    std::vector<std::string> const &modes)
{
  outer_.modes_ = modes;
}

AMD::PMPowerStateProfilePart::PMPowerStateProfilePart() noexcept
: id_(AMD::PMPowerState::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMPowerStateProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerStateProfilePart::initializer()
{
  return std::make_unique<AMD::PMPowerStateProfilePart::Initializer>(*this);
}

std::string const &AMD::PMPowerStateProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerStateProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMPowerStateProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMPowerStateProfilePart::providePMPowerStateMode() const
{
  return mode_;
}

void AMD::PMPowerStateProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmfImporter = dynamic_cast<AMD::PMPowerStateProfilePart::Importer &>(i);
  mode(pmfImporter.providePMPowerStateMode());
}

void AMD::PMPowerStateProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmfExporter = dynamic_cast<AMD::PMPowerStateProfilePart::Exporter &>(e);
  pmfExporter.takePMPowerStateMode(mode_);
}

std::unique_ptr<IProfilePart> AMD::PMPowerStateProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMPowerStateProfilePart>();
  clone->modes_ = modes_;
  clone->mode_ = mode_;

  return std::move(clone);
}

void AMD::PMPowerStateProfilePart::mode(std::string const &mode)
{
  // import known modes
  auto iter = std::find_if(
      modes_.cbegin(), modes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != modes_.cend())
    mode_ = mode;
}

bool const AMD::PMPowerStateProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMPowerState::ItemID, []() {
      return std::make_unique<AMD::PMPowerStateProfilePart>();
    });
