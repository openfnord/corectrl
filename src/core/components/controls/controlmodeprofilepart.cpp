// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "controlmodeprofilepart.h"

#include "core/item.h"
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

class ControlModeProfilePart::Factory final
: public ProfilePart::Factory
, public ControlMode::Exporter
{
 public:
  Factory(IProfilePartProvider const &profilePartProvider,
          ControlModeProfilePart &outer) noexcept
  : ProfilePart::Factory(profilePartProvider)
  , outer_(outer)
  {
  }

  void takeProfilePart(std::unique_ptr<IProfilePart> &&part) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override
  {
  }

  void takeMode(std::string const &) override
  {
  }

  void takeModes(std::vector<std::string> const &) override
  {
  }

 private:
  ControlModeProfilePart &outer_;
};

void ControlModeProfilePart::Factory::takeProfilePart(
    std::unique_ptr<IProfilePart> &&part)
{
  outer_.parts_.emplace_back(std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeProfilePart::Factory::provideExporter(Item const &i)
{
  return factory(i.ID());
}

class ControlModeProfilePart::Initializer final : public ControlMode::Exporter
{
 public:
  Initializer(ControlModeProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override;
  void takeMode(std::string const &) override;
  void takeModes(std::vector<std::string> const &) override
  {
  }

 private:
  ControlModeProfilePart &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeProfilePart::Initializer::provideExporter(Item const &i)
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

void ControlModeProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void ControlModeProfilePart::Initializer::takeMode(std::string const &mode)
{
  outer_.mode_ = mode;
}

ControlModeProfilePart::ControlModeProfilePart(std::string_view id) noexcept
: id_(id)
{
}

std::unique_ptr<Exportable::Exporter>
ControlModeProfilePart::factory(IProfilePartProvider const &profilePartProvider)
{
  return std::make_unique<ControlModeProfilePart::Factory>(profilePartProvider,
                                                           *this);
}

std::unique_ptr<Exportable::Exporter> ControlModeProfilePart::initializer()
{
  return std::make_unique<ControlModeProfilePart::Initializer>(*this);
}

std::string const &ControlModeProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlModeProfilePart::provideImporter(Item const &i)
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

bool ControlModeProfilePart::provideActive() const
{
  return active();
}

std::string const &ControlModeProfilePart::provideMode() const
{
  return mode_;
}

void ControlModeProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmImporter = dynamic_cast<ControlModeProfilePart::Importer &>(i);
  mode(pmImporter.provideMode());

  for (auto &part : parts_) {
    part->importWith(i);

    // only the selected mode profile part can be active
    part->activate(part->ID() == mode_);
  }
}

void ControlModeProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmExporter = dynamic_cast<ControlModeProfilePart::Exporter &>(e);
  pmExporter.takeMode(mode_);

  for (auto &part : parts_)
    part->exportWith(e);
}

std::unique_ptr<IProfilePart> ControlModeProfilePart::cloneProfilePart() const
{
  auto clone = instance();

  clone->parts_.reserve(parts_.size());
  std::transform(
      parts_.cbegin(), parts_.cend(), std::back_inserter(clone->parts_),
      [](std::unique_ptr<IProfilePart> const &part) { return part->clone(); });

  clone->mode_ = mode_;

  return std::move(clone);
}

void ControlModeProfilePart::mode(std::string const &mode)
{
  auto iter = std::find_if(parts_.cbegin(), parts_.cend(),
                           [&](auto &part) { return mode == part->ID(); });
  if (iter != parts_.cend())
    mode_ = mode;
}
