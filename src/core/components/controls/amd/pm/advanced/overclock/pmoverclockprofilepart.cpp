// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverclockprofilepart.h"

#include "core/profilepartprovider.h"
#include "pmoverclock.h"
#include <memory>

AMD::PMOverclockProfilePart::PMOverclockProfilePart() noexcept
: ControlGroupProfilePart(AMD::PMOverclock::ItemID)
{
}

std::unique_ptr<ControlGroupProfilePart> AMD::PMOverclockProfilePart::instance() const
{
  return std::make_unique<AMD::PMOverclockProfilePart>();
}

bool const AMD::PMOverclockProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMOverclock::ItemID, []() {
      return std::make_unique<AMD::PMOverclockProfilePart>();
    });
