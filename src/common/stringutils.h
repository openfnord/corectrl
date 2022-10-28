// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <exception>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace Utils {
namespace String {

/// Cleans a prefix from a string.
///
/// @returns substring of the input string without prefix
std::string cleanPrefix(std::string const &str, std::string const &prefix);

/// Converts a string in 'mayor.minor.patch' format into numeric value.
///
/// @param version string in 'mayor.minor.patch' format
/// @returns version [mayor, minor, patch] as integers or [0, 0, 0] on error
std::tuple<int, int, int> parseVersion(std::string const &version);

/// Converts a string containing a number representation into its numerical value.
/// @param value will store the result of the conversion
/// @param rep representation of the number to convert
/// @param base numerical base of the representation to convert from (for integers)
/// @returns true on a successful conversion
template<typename T>
bool toNumber(T &value, std::string const &rep, int base = 10)
{
  try {
    if constexpr (std::is_same_v<T, char>) {
      value = static_cast<char>(std::stoi(rep, nullptr, base));
    }
    else if constexpr (std::is_same_v<T, short>) {
      value = static_cast<short>(std::stoi(rep, nullptr, base));
    }
    else if constexpr (std::is_same_v<T, int>)
      value = std::stoi(rep, nullptr, base);
    else if constexpr (std::is_same_v<T, long>)
      value = std::stol(rep, nullptr, base);
    else if constexpr (std::is_same_v<T, long long>)
      value = std::stoll(rep, nullptr, base);
    else if constexpr (std::is_same_v<T, unsigned char> ||
                       std::is_same_v<T, unsigned short> ||
                       std::is_same_v<T, unsigned int> ||
                       std::is_same_v<T, unsigned long>)
      value = static_cast<T>(std::stoul(rep, nullptr, base));
    else if constexpr (std::is_same_v<T, unsigned long long>)
      value = std::stoull(rep, nullptr, base);
    else if constexpr (std::is_same_v<T, float>)
      value = std::stof(rep, nullptr);
    else if constexpr (std::is_same_v<T, double>)
      value = std::stod(rep, nullptr);
    else if constexpr (std::is_same_v<T, long double>)
      value = std::stold(rep, nullptr);

    return true;
  }
  catch (std::exception const &e) {
    LOG(ERROR) << fmt::format(
        "Cannot parse a number from the string '{}'. Error: {}", rep.c_str(),
        e.what());
  }

  return false;
}

/// Splits a string into substrings using a delimiter.
/// @param src string to be splitted
/// @param delim delimiter to cut into substrings
/// @returns collection of substrings
std::vector<std::string> split(std::string const &src, char delim = ' ');

/// Parse kernel version from a string following the format used by the kernel
/// on /proc/version.
/// @param data string to parse the kernel version from
/// @returns a string containing the kernel version following semver format
std::optional<std::string> parseKernelProcVersion(std::string const &data);

} // namespace String
} // namespace Utils
