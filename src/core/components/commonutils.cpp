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
#include "commonutils.h"

#include "common/mathutils.h"
#include <algorithm>
#include <cstddef>
#include <iterator>

void Utils::Common::normalizePoints(
    std::vector<std::pair<units::temperature::celsius_t,
                          units::concentration::percent_t>> &points,
    std::pair<units::temperature::celsius_t, units::temperature::celsius_t> range)
{
  std::vector<double> pTemps;

  if (std::any_of(points.cbegin(), points.cend(), [&](auto &point) {
        return point.first < range.first || point.first > range.second;
      })) {
    std::transform(points.cbegin(), points.cend(), std::back_inserter(pTemps),
                   [](auto &point) { return point.first.template to<double>(); });

    auto [minTemp, maxTemp] = std::minmax_element(pTemps.cbegin(), pTemps.cend());

    Utils::Math::linearNorm(
        pTemps,
        std::make_pair(std::min(0.0, *minTemp), std::max(90.0, *maxTemp)),
        std::make_pair(range.first.to<double>(), range.second.to<double>()));
  }

  for (size_t i = 0; i < points.size(); ++i) {
    auto &[pTemp, pPwm] = points.at(i);

    // always clamp pwm value
    pPwm = std::clamp(pPwm, units::concentration::percent_t(0),
                      units::concentration::percent_t(100));

    if (!pTemps.empty())
      pTemp = units::temperature::celsius_t(pTemps[i]);
  }
}
