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

#include "ippdpmhandler.h"
#include "units/units.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class PpDpmHandler : public IPpDpmHandler
{
 public:
  PpDpmHandler(std::unique_ptr<IDataSource<std::vector<std::string>>>
                   &&ppDpmDataSource) noexcept;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &
  states() const override;

  std::vector<unsigned int> const &active() const override;
  void activate(std::vector<unsigned int> const &states) override;

  void saveState() override;
  void restoreState(ICommandQueue &ctlCmds) override;

  void reset(ICommandQueue &ctlCmds) override;
  void apply(ICommandQueue &ctlCmds) override;
  void sync(ICommandQueue &ctlCmds) override;

 private:
  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppDpmDataSource_;
  std::vector<std::string> ppDpmLines_;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states_;
  std::vector<unsigned int> active_;

  bool resync_;
};

} // namespace AMD
