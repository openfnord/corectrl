// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmadvancedprofilepart.h"

#include "core/profilepartprovider.h"
#include "pmadvanced.h"
#include <memory>

AMD::PMAdvancedProfilePart::PMAdvancedProfilePart() noexcept
: ControlGroupProfilePart(AMD::PMAdvanced::ItemID)
{
}

std::unique_ptr<ControlGroupProfilePart> AMD::PMAdvancedProfilePart::instance() const
{
  return std::make_unique<AMD::PMAdvancedProfilePart>();
}

bool const AMD::PMAdvancedProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMAdvanced::ItemID, []() {
      return std::make_unique<AMD::PMAdvancedProfilePart>();
    });
