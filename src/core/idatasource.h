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
  virtual bool read(Ts &... args) = 0;

  virtual ~IDataSource() = default;
};
