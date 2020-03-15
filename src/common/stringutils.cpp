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
#include "stringutils.h"

#include <algorithm>
#include <cstddef>
#include <sstream>

namespace Utils {
namespace String {

std::string cleanPrefix(std::string const &str, std::string const &prefix)
{
  auto it = std::search(str.cbegin(), str.cend(), prefix.cbegin(), prefix.cend());
  if (it != str.cend()) {
    size_t const pos = prefix.size();
    return str.substr(pos, str.size() - pos);
  }

  return str;
}

std::tuple<int, int, int> parseVersion(std::string const &version)
{
  try {
    if (std::count(version.cbegin(), version.cend(), '.') == 2) {
      auto firstDot = version.find(".");
      auto secondDot = version.find(".", firstDot + 1);
      if (firstDot > 0 &&                   // mayor version exists
          firstDot + 1 < secondDot &&       // minor version exists
          secondDot < version.length() - 1) // patch version exists
        return {std::stoi(version.substr(0, firstDot)),
                std::stoi(version.substr(firstDot + 1, secondDot)),
                std::stoi(version.substr(secondDot + 1, std::string::npos))};
    }
    LOG(ERROR) << fmt::format("'{}' is not a valid version string",
                              version.c_str());
  }
  catch (std::exception const &e) {
    LOG(ERROR) << fmt::format("Cannot parse version string '{}'. Error: {}",
                              version.c_str(), e.what());
  }

  return {0, 0, 0};
}

std::vector<std::string> split(std::string const &src, char delim)
{
  std::vector<std::string> result;

  std::istringstream istream(src);
  for (std::string part; std::getline(istream, part, delim);)
    if (!part.empty()) // skip empty parts
      result.push_back(part);

  return result;
}

} // namespace String
} // namespace Utils
