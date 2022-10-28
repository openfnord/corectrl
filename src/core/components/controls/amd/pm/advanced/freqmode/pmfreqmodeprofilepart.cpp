// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqmodeprofilepart.h"

#include "core/profilepartprovider.h"
#include "pmfreqmode.h"
#include <memory>

AMD::PMFreqModeProfilePart::PMFreqModeProfilePart() noexcept
: ControlModeProfilePart(AMD::PMFreqMode::ItemID)
{
}

std::unique_ptr<ControlModeProfilePart> AMD::PMFreqModeProfilePart::instance() const
{
  return std::make_unique<AMD::PMFreqModeProfilePart>();
}

bool const AMD::PMFreqModeProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFreqMode::ItemID, []() {
      return std::make_unique<AMD::PMFreqModeProfilePart>();
    });
