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
