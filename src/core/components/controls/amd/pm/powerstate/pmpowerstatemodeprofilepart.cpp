// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerstatemodeprofilepart.h"

#include "core/profilepartprovider.h"
#include "pmpowerstatemode.h"
#include <memory>

AMD::PMPowerStateModeProfilePart::PMPowerStateModeProfilePart() noexcept
: ControlModeProfilePart(AMD::PMPowerStateMode::ItemID)
{
}

std::unique_ptr<ControlModeProfilePart>
AMD::PMPowerStateModeProfilePart::instance() const
{
  return std::make_unique<AMD::PMPowerStateModeProfilePart>();
}

bool const AMD::PMPowerStateModeProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMPowerStateMode::ItemID, []() {
      return std::make_unique<AMD::PMPowerStateModeProfilePart>();
    });
