// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/components/controls/cpu/cpufreqmode.h"

namespace Tests {
namespace CPUFreqMode {

TEST_CASE("AMD CPUFreqMode tests", "[CPU][CPUFreqMode]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has CPUFreqMode ID")
  {
    ::CPUFreqMode ts(std::move(controlMocks));
    REQUIRE(ts.ID() == ::CPUFreqMode::ItemID);
  }

  SECTION("Is active by default")
  {
    ::CPUFreqMode ts(std::move(controlMocks));
    REQUIRE(ts.active());
  }
}
} // namespace CPUFreqMode
} // namespace Tests
