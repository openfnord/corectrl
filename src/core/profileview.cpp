// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profileview.h"

#include "iprofilepart.h"
#include "isyscomponentprofilepart.h"
#include "isysmodel.h"
#include "profilepartview.h"
#include <algorithm>
#include <utility>

std::string const &ProfileView::name() const
{
  return name_;
}

std::vector<std::unique_ptr<IProfilePartView>> const &ProfileView::parts() const
{
  return parts_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
ProfileView::provideImporter(Item const &i)
{
  if (i.ID() == ISysModel::ItemID)
    return *this;
  else {
    auto partIter = std::find_if(
        parts_.cbegin(), parts_.cend(),
        [&](std::unique_ptr<IProfilePartView> const &part) {
          auto *sysPart = dynamic_cast<ISysComponentProfilePart const *>(
              part->part().get());
          if (sysPart == nullptr)
            return false;

          return sysPart->belongsTo(i);
        });
    if (partIter != parts_.cend())
      return dynamic_cast<Importable::Importer &>(*partIter->get()->part());
  }

  return {};
}

ProfileView::Initializer::Initializer(
    ProfileView &profileView,
    std::optional<std::reference_wrapper<IProfileView>> base) noexcept
: profileView_(profileView)
, base_(base)
{
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ProfileView::Initializer::provideExporter(Item const &i)
{
  if (i.ID() == IProfile::ItemID) {
    auto &pView = dynamic_cast<IProfileView::View const &>(i);
    profileView_.name_ = pView.name();

    auto &parts = pView.parts();
    for (auto &part : parts) {
      std::shared_ptr<IProfilePart> profilePart;
      std::string partProfileName;

      if (!part->active() && base_.has_value()) { // compose from base_
        partProfileName = base_->get().name();

        auto &baseParts = base_->get().parts();
        auto const basePartIter = std::find_if(
            baseParts.cbegin(), baseParts.cend(), [&](auto &partView) {
              return partView->part()->ID() == part->ID();
            });
        if (basePartIter != baseParts.cend())
          profilePart = basePartIter->get()->part();
      }
      else {
        partProfileName = dynamic_cast<IProfile const &>(i).info().name;
        profilePart = part;
      }

      profileView_.parts_.emplace_back(std::make_unique<ProfilePartView>(
          std::move(partProfileName), std::move(profilePart)));
    }
  }

  return {};
}

void ProfileView::Initializer::takeActive(bool)
{
}

void ProfileView::Initializer::takeInfo(IProfile::Info const &)
{
}
