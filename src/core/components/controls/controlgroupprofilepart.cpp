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
#include "controlgroupprofilepart.h"

#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

class ControlGroupProfilePart::Factory final
: public ProfilePart::Factory
, public ControlGroup::Exporter
{
 public:
  Factory(IProfilePartProvider const &profilePartProvider,
          ControlGroupProfilePart &profilePart) noexcept
  : ProfilePart::Factory(profilePartProvider)
  , outer_(profilePart)
  {
  }

  void takeProfilePart(std::unique_ptr<IProfilePart> &&part) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override
  {
  }

 private:
  ControlGroupProfilePart &outer_;
};

void ControlGroupProfilePart::Factory::takeProfilePart(
    std::unique_ptr<IProfilePart> &&part)
{
  outer_.parts_.emplace_back(std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupProfilePart::Factory::provideExporter(Item const &i)
{
  return factory(i.ID());
}

class ControlGroupProfilePart::Initializer final : public ControlGroup::Exporter
{
 public:
  Initializer(ControlGroupProfilePart &profilePart) noexcept
  : outer_(profilePart)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;

 private:
  ControlGroupProfilePart &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupProfilePart::Initializer::provideExporter(Item const &i)
{
  for (auto &part : outer_.parts_) {
    auto &id = part->ID();
    if (id == i.ID()) {
      if (initializers_.count(id) > 0)
        return *initializers_.at(id);
      else {
        auto initializer = part->initializer();
        if (initializer != nullptr) {
          initializers_.emplace(id, std::move(initializer));
          return *initializers_.at(id);
        }
      }

      break;
    }
  }

  return {};
}

void ControlGroupProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

ControlGroupProfilePart::ControlGroupProfilePart(std::string_view id) noexcept
: id_(id)
{
}

std::unique_ptr<Exportable::Exporter>
ControlGroupProfilePart::factory(IProfilePartProvider const &profilePartProvider)
{
  return std::make_unique<ControlGroupProfilePart::Factory>(profilePartProvider,
                                                            *this);
}

std::unique_ptr<Exportable::Exporter> ControlGroupProfilePart::initializer()
{
  return std::make_unique<ControlGroupProfilePart::Initializer>(*this);
}

std::string const &ControlGroupProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlGroupProfilePart::provideImporter(Item const &i)
{
  auto &id = i.ID();
  auto partIter = std::find_if(parts_.cbegin(), parts_.cend(),
                               [&](auto &part) { return part->ID() == id; });

  if (partIter != parts_.cend()) {
    auto importer = dynamic_cast<Importable::Importer *>(partIter->get());
    if (importer != nullptr)
      return *importer;
  }

  return {};
}

bool ControlGroupProfilePart::provideActive() const
{
  return active();
}

void ControlGroupProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  for (auto &part : parts_) {
    part->importWith(i);

    // ensure that all parts are active
    part->activate(true);
  }
}

void ControlGroupProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  for (auto &part : parts_)
    part->exportWith(e);
}

std::unique_ptr<IProfilePart> ControlGroupProfilePart::cloneProfilePart() const
{
  auto clone = instance();
  clone->parts_.reserve(parts_.size());
  std::transform(
      parts_.cbegin(), parts_.cend(), std::back_inserter(clone->parts_),
      [](std::unique_ptr<IProfilePart> const &part) { return part->clone(); });

  return std::move(clone);
}
