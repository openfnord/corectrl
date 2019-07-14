//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
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

#include "core/components/controls/amd/pm/advanced/pmadvanced.h"

namespace Tests {
namespace AMD {
namespace PMAdvanced {

TEST_CASE("AMD PMAdvanced tests", "[GPU][AMD][PM][PMAdvanced]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has PMAdvanced ID")
  {
    ::AMD::PMAdvanced ts(std::move(controlMocks));
    REQUIRE(ts.ID() == ::AMD::PMAdvanced::ItemID);
  }

  SECTION("Is not active by default")
  {
    ::AMD::PMAdvanced ts(std::move(controlMocks));
    REQUIRE_FALSE(ts.active());
  }
}
} // namespace PMAdvanced
} // namespace AMD
} // namespace Tests
