// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <filesystem>
#include <string>

class ISysfsWriter
{
 public:
  virtual void write(std::filesystem::path const &sysfsEntry,
                     std::string const &value) = 0;

  virtual ~ISysfsWriter() = default;
};
