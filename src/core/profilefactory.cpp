// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilefactory.h"

#include "isysmodel.h"
#include "profile.h"
#include <utility>

ProfileFactory::ProfileFactory(
    std::unique_ptr<IProfilePartProvider> &&profilePartProvider) noexcept
: profilePartProvider_(std::move(profilePartProvider))
{
}

std::unique_ptr<IProfile> ProfileFactory::build(ISysModel const &sysModel) const
{
  auto profile = std::make_unique<Profile>();
  Profile::Factory profileFactory(*profilePartProvider_, *profile);
  sysModel.exportWith(profileFactory);
  return std::move(profile);
}
