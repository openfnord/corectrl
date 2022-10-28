// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

/// Interface of a data sink.
/// A data sink only receives data.
template<typename... Ts>
class IDataSink
{
 public:
  /// Where the data goes to.
  /// @returns sink name
  virtual std::string sink() const = 0;

  /// Write the data to the sink.
  /// @return true when the operation succeed
  virtual bool write(Ts const &...args) = 0;

  virtual ~IDataSink() = default;
};
