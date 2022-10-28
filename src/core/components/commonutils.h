// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
