// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/idatasource.h"

class StringDataSourceStub : public IDataSource<std::string>
{
 public:
  StringDataSourceStub(std::string_view source = "", std::string_view data = "",
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

  bool read(std::string &data) override
  {
    data = data_;
    return success_;
  }

  std::string const source_;
  std::string const data_;
  bool success_;
};
