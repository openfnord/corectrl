// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmperfmodeprofilepart.h"

#include "core/profilepartprovider.h"
#include "pmperfmode.h"
#include <memory>

AMD::PMPerfModeProfilePart::PMPerfModeProfilePart() noexcept
: ControlModeProfilePart(AMD::PMPerfMode::ItemID)
{
}

std::unique_ptr<ControlModeProfilePart> AMD::PMPerfModeProfilePart::instance() const
{
  return std::make_unique<AMD::PMPerfModeProfilePart>();
}

bool const AMD::PMPerfModeProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMPerfMode::ItemID, []() {
      return std::make_unique<AMD::PMPerfModeProfilePart>();
    });
