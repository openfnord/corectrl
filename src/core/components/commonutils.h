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

#include "units/units.h"
#include <utility>
#include <vector>

namespace Utils {
namespace Common {

/// Normalizes points into temperature and percentage ranges.
/// @note Temperature normalization is only performed when needed.
/// @param points points to normalize
/// @param range target temperature range
void normalizePoints(
    std::vector<std::pair<units::temperature::celsius_t,
                          units::concentration::percent_t>> &points,
    std::pair<units::temperature::celsius_t, units::temperature::celsius_t> range);

} // namespace Common
} // namespace Utils
