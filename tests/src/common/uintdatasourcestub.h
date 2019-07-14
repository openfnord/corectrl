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

#include "core/idatasource.h"

class UIntDataSourceStub : public IDataSource<unsigned int>
{
 public:
  UIntDataSourceStub(std::string_view source = "", unsigned int data = 0,
                     bool success = true) noexcept
  : source_(source)
  , data_(data)
  , success_(success)
  {
  }

  std::string source() const override
  {
    return source_;
  }

  bool read(unsigned int &data) override
  {
    data = data_;
    return success_;
  }

  void data(unsigned int data)
  {
    data_ = data;
  }

  std::string const source_;
  unsigned int data_;
  bool success_;
};
