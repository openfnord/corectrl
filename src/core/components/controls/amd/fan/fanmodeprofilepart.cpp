// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanmodeprofilepart.h"

#include "core/profilepartprovider.h"
#include "fanmode.h"
#include <memory>

AMD::FanModeProfilePart::FanModeProfilePart() noexcept
: ControlModeProfilePart(AMD::FanMode::ItemID)
{
}

std::unique_ptr<ControlModeProfilePart> AMD::FanModeProfilePart::instance() const
{
  return std::make_unique<AMD::FanModeProfilePart>();
}

bool const AMD::FanModeProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::FanMode::ItemID, []() {
      return std::make_unique<AMD::FanModeProfilePart>();
    });
