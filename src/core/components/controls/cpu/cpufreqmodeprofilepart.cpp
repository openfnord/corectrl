// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpufreqmodeprofilepart.h"

#include "core/profilepartprovider.h"
#include "cpufreqmode.h"
#include <memory>

CPUFreqModeProfilePart::CPUFreqModeProfilePart() noexcept
: ControlModeProfilePart(CPUFreqMode::ItemID)
{
}

std::unique_ptr<ControlModeProfilePart> CPUFreqModeProfilePart::instance() const
{
  return std::make_unique<CPUFreqModeProfilePart>();
}

bool const CPUFreqModeProfilePart::registered_ =
    ProfilePartProvider::registerProvider(CPUFreqMode::ItemID, []() {
      return std::make_unique<CPUFreqModeProfilePart>();
    });
