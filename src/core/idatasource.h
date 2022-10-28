// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

/// Interface of a data source.
/// A data source only provides data.
template<typename... Ts>
class IDataSource
{
 public:
  /// Where the data comes from.
  /// @returns source name
  virtual std::string source() const = 0;

  /// Read the data from the source. The readed data is stored into
  /// this function arguments.
  /// @return true when the operation succeed
  virtual bool read(Ts &...args) = 0;

  virtual ~IDataSource() = default;
};
