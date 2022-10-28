// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profileviewfactory.h"

#include "exportable.h"
#include "profileview.h"
#include <utility>

std::unique_ptr<IProfileView> ProfileViewFactory::build(
    Exportable const &profile,
    std::optional<std::reference_wrapper<IProfileView>> base) const
{
  auto profileView = std::make_unique<ProfileView>();
  ProfileView::Initializer pvInitializer(*profileView, base);
  profile.exportWith(pvInitializer);
  return std::move(profileView);
}
