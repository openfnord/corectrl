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

class Item
{
 public:
  virtual std::string const &ID() const = 0;

  /// Returns the instance ID.
  ///
  /// When there are multiple instances of the same item, an unique
  /// instance ID will be returned for each item. Otherwise, the
  /// returned instance ID will be the value of the item ID.
  virtual std::string const &instanceID() const
  {
    return ID();
  }

  virtual ~Item() = default;
};
