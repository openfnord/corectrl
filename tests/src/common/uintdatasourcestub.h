// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
