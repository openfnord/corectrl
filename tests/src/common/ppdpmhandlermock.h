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
#pragma once

#include "core/components/controls/amd/pm/handlers/ippdpmhandler.h"
#include "trompeloeil.hpp"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {

class PpDpmHandlerMock : public ::AMD::IPpDpmHandler
{
 public:
  PpDpmHandlerMock(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states =
          {})
  : states_(std::move(states))
  {
  }

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &
  states() const override
  {
    return states_;
  }

  MAKE_CONST_MOCK0(active, std::vector<unsigned int> const &(), override);
  MAKE_MOCK1(activate, void(std::vector<unsigned int> const &), override);
  MAKE_MOCK0(saveState, void(), override);
  MAKE_MOCK1(restoreState, void(ICommandQueue &), override);
  MAKE_MOCK1(reset, void(ICommandQueue &), override);
  MAKE_MOCK1(apply, void(ICommandQueue &), override);
  MAKE_MOCK1(sync, void(ICommandQueue &), override);

 private:
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const states_;
};

} // namespace AMD
} // namespace Tests
