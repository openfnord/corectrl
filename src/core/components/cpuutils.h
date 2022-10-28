// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Utils {
namespace CPU {

/// Returns a piece of information from /proc/cpuinfo.
/// @param procCpuInfoLines /proc/cpuinfo data source contents lines
/// @param cpuId cpu id from which the info will be retrieved
/// @param target label that describes the information to retrieve
std::optional<std::string>
parseProcCpuInfo(std::vector<std::string> const &procCpuInfoLines, int cpuId,
                 std::string_view target);

} // namespace CPU
} // namespace Utils
