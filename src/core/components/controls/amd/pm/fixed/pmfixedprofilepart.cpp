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
#include "pmfixedprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>
#include <utility>

class AMD::PMFixedProfilePart::Initializer final : public AMD::PMFixed::Exporter
{
 public:
  Initializer(AMD::PMFixedProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFixedMode(std::string const &mode) override;
  void takePMFixedModes(std::vector<std::string> const &modes) override;

 private:
  AMD::PMFixedProfilePart &outer_;
};

void AMD::PMFixedProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFixedProfilePart::Initializer::takePMFixedMode(std::string const &mode)
{
  outer_.mode_ = mode;
}

void AMD::PMFixedProfilePart::Initializer::takePMFixedModes(
    std::vector<std::string> const &modes)
{
  outer_.modes_ = modes;
}

AMD::PMFixedProfilePart::PMFixedProfilePart() noexcept
: id_(AMD::PMFixed::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFixedProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFixedProfilePart::initializer()
{
  return std::make_unique<AMD::PMFixedProfilePart::Initializer>(*this);
}

std::string const &AMD::PMFixedProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFixedProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMFixedProfilePart::providePMFixedMode() const
{
  return mode_;
}

void AMD::PMFixedProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmfImporter = dynamic_cast<AMD::PMFixedProfilePart::Importer &>(i);
  mode(pmfImporter.providePMFixedMode());
}

void AMD::PMFixedProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmfExporter = dynamic_cast<AMD::PMFixedProfilePart::Exporter &>(e);
  pmfExporter.takePMFixedMode(mode_);
}

std::unique_ptr<IProfilePart> AMD::PMFixedProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFixedProfilePart>();
  clone->modes_ = modes_;
  clone->mode_ = mode_;

  return std::move(clone);
}

void AMD::PMFixedProfilePart::mode(std::string const &mode)
{
  auto iter = std::find_if(
      modes_.cbegin(), modes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != modes_.cend())
    mode_ = mode;
}

bool const AMD::PMFixedProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFixed::ItemID, []() {
      return std::make_unique<AMD::PMFixedProfilePart>();
    });
