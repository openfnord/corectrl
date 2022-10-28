// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
