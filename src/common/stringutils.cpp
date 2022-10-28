// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "stringutils.h"

#include <algorithm>
#include <cstddef>
#include <regex>
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

std::optional<std::string> parseKernelProcVersion(std::string const &data)
{
  std::regex const regex(R"(^Linux\s*version\s*(\d+\.\d+(?:\.\d+){0,1}).*)");

  std::smatch result;
  if (!std::regex_search(data, result, regex)) {
    LOG(ERROR) << "Cannot parse kernel version";
    return {};
  }

  // Append .0 when the patch version number is missing, see #254
  std::string version = result[1];
  if (std::count(version.cbegin(), version.cend(), '.') == 1)
    version.append(".0");

  return version;
}

} // namespace String
} // namespace Utils
