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

#include "core/components/controls/idatasourcehandler.h"
#include "units/units.h"
#include <utility>
#include <vector>

namespace AMD {

class IPpDpmHandler : public IDataSourceHandler
{
 public:
  virtual std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &
  states() const = 0;

  virtual std::vector<unsigned int> const &active() const = 0;
  virtual void activate(std::vector<unsigned int> const &states) = 0;

  virtual ~IPpDpmHandler() = default;
};

} // namespace AMD
