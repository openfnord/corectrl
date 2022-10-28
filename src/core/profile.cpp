// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profile.h"

#include "iprofile.h"
#include "iprofilepart.h"
#include "iprofilepartprovider.h"
#include "isyscomponent.h"
#include "isyscomponentprofilepart.h"
#include "isysmodel.h"
#include "item.h"
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

Profile::Factory::Factory(IProfilePartProvider const &profilePartProvider,
                          Profile &outer) noexcept
: ProfilePart::Factory(profilePartProvider)
, outer_(outer)
{
}

void Profile::Factory::takeProfilePart(std::unique_ptr<IProfilePart> &&part)
{
  outer_.parts_.emplace_back(std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
Profile::Factory::provideExporter(Item const &i)
{
  auto &id = i.ID();
  if (id == ISysModel::ItemID)
    return *this;
  else
    return factory(id);
}

class Profile::Initializer final : public ISysModel::Exporter
{
 public:
  Initializer(Profile const &outer)
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

 private:
  Profile const &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
Profile::Initializer::provideExporter(Item const &i)
{
  auto &id = i.ID();
  if (id == ISysModel::ItemID)
    return *this;
  else {
    auto &key = dynamic_cast<ISysComponent const &>(i).key();
    for (auto &part : outer_.parts_) {
      if (key == dynamic_cast<ISysComponentProfilePart const &>(*part).key()) {
        if (initializers_.count(key) > 0)
          return *initializers_.at(key);
        else {
          auto initializer = part->initializer();
          if (initializer != nullptr) {
            initializers_.emplace(key, std::move(initializer));
            return *initializers_.at(key);
          }
        }

        break;
      }
    }
  }

  return {};
}

Profile::Profile() noexcept
: id_(IProfile::ItemID)
{
}

std::unique_ptr<Exportable::Exporter> Profile::initializer()
{
  return std::make_unique<Profile::Initializer>(*this);
}

bool Profile::active() const
{
  return active_;
}

void Profile::activate(bool active)
{
  active_ = active;
}

IProfile::Info const &Profile::info() const
{
  return info_;
}

void Profile::info(Info const &info)
{
  info_ = info;
}

std::unique_ptr<IProfile> Profile::clone() const
{
  auto clone = std::make_unique<Profile>();
  clone->active_ = active_;
  clone->info_ = info_;
  clone->parts_.reserve(parts_.size());
  std::transform(
      parts_.cbegin(), parts_.cend(), std::back_inserter(clone->parts_),
      [](std::shared_ptr<IProfilePart> const &part) { return part->clone(); });

  return std::move(clone);
}

std::string const &Profile::ID() const
{
  return id_;
}

void Profile::importWith(Importable::Importer &i)
{
  auto importer = i.provideImporter(*this);
  if (importer.has_value()) {
    auto &profileImporter = dynamic_cast<IProfile::Importer &>(importer->get());

    activate(profileImporter.provideActive());
    info(profileImporter.provideInfo());

    for (auto &part : parts_)
      part->importWith(*importer);
  }
}

void Profile::exportWith(Exportable::Exporter &e) const
{
  auto exporter = e.provideExporter(*this);
  if (exporter.has_value()) {
    auto &profileExporter = dynamic_cast<IProfile::Exporter &>(exporter->get());

    profileExporter.takeActive(active());
    profileExporter.takeInfo(info());

    for (auto &part : parts_)
      part->exportWith(*exporter);
  }
}

std::string const &Profile::name() const
{
  return info_.name;
}

std::vector<std::shared_ptr<IProfilePart>> const &Profile::parts() const
{
  return parts_;
}
