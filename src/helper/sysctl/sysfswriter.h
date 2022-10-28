// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "isysfswriter.h"
#include <filesystem>
#include <string_view>

class SysfsWriter final : public ISysfsWriter
{
 public:
  void write(std::filesystem::path const &sysfsEntry,
             std::string const &value) override;

 private:
  static constexpr std::string_view sysfsPath{"/sys"};

  bool isSysfsPath(std::filesystem::path const &path) const;
};
