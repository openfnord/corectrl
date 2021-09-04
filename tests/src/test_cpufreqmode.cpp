//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
