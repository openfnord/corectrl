// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <algorithm>
#include <utility>
#include <vector>

namespace Utils {
namespace Math {

/// Performs a linear normalization to a collection of values from an old range
/// to a new range.
///
/// @note Only works with regular numeric types.
///
/// @param values collection of values to normalize
/// @param oldRange old range of the values of the collection
/// @param newRange new range for the result of the normalization
template<typename T>
void linearNorm(std::vector<T> &values, std::pair<T, T> const &oldRange,
                std::pair<T, T> const &newRange)
{
  std::transform(values.cbegin(), values.cend(), values.begin(), [&](T value) {
    return static_cast<T>(
        (value - oldRange.first) *
            (static_cast<double>(newRange.second - newRange.first) /
             (oldRange.second - oldRange.first)) +
        newRange.first);
  });
}

/// Performs a lineal interpolation between two points resolving the y value at
/// the supplied x value.
///
/// @note Only works with regular numeric types.
///
/// @param value x value to be evaluated
/// @param p1 first point
/// @param p2 second point
/// @return resolved y value
template<typename X, typename Y>
Y lerpX(X value, std::pair<X, Y> const &p1, std::pair<X, Y> const &p2)
{
  return static_cast<X>(
      (static_cast<double>(p2.second - p1.second) / (p2.first - p1.first)) *
          (value - p1.first) +
      p1.second);
}

/// Performs a lineal interpolation between two points resolving the x value at
/// the supplied y value.
///
/// @note Only works with regular numeric types.
///
/// @param value y value to be evaluated
/// @param p1 first point
/// @param p2 second point
/// @return resolved x value
template<typename X, typename Y>
X lerpY(Y value, std::pair<X, Y> const &p1, std::pair<X, Y> const &p2)
{
  return static_cast<Y>(
      (static_cast<double>(p2.first - p1.first) / (p2.second - p1.second)) *
          (value - p1.second) +
      p1.first);
}

} // namespace Math
} // namespace Utils
