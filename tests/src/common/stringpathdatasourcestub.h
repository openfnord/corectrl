// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/idatasource.h"
#include <filesystem>

class StringPathDataSourceStub
: public IDataSource<std::string, std::filesystem::path const>
{
 public:
  StringPathDataSourceStub(std::string_view path = "",
                           std::string_view data = "",
                           bool success = true) noexcept
  : source_(path)
  , data_(data)
  , success_(success)
  {
  }

  std::string source() const override
  {
    return source_;
  }

  bool read(std::string &data, std::filesystem::path const &) override
  {
    data = data_;
    return success_;
  }

  std::string const source_;
  std::string const data_;
  bool success_;
};
