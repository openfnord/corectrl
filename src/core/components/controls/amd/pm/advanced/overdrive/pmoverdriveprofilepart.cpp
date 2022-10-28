// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverdriveprofilepart.h"

#include "core/profilepartprovider.h"
#include "pmoverdrive.h"
#include <memory>

AMD::PMOverdriveProfilePart::PMOverdriveProfilePart() noexcept
: ControlGroupProfilePart(AMD::PMOverdrive::ItemID)
{
}

std::unique_ptr<ControlGroupProfilePart> AMD::PMOverdriveProfilePart::instance() const
{
  return std::make_unique<AMD::PMOverdriveProfilePart>();
}

bool const AMD::PMOverdriveProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMOverdrive::ItemID, []() {
      return std::make_unique<AMD::PMOverdriveProfilePart>();
    });
