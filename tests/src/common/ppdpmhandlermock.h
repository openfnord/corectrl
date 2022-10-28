// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
  MAKE_MOCK1(sync, void(ICommandQueue &), override);

 private:
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const states_;
};

} // namespace AMD
} // namespace Tests
