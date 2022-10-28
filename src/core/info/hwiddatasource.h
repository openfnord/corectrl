// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/idatasource.h"
#include <string>
#include <vector>

class HWIDDataSource : public IDataSource<std::vector<char>>
{
 public:
  HWIDDataSource(std::string const &path) noexcept;

  std::string source() const override;
  bool read(std::vector<char> &data) override;

 private:
  std::string const path_;
};
