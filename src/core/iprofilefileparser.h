// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class IProfile;

class IProfileFileParser
{
 public:
  static constexpr std::string_view ProfileDataFileName{"profile"};
  static constexpr std::string_view IconDataFileName{"icon"};

  virtual std::string fileExtension() const = 0;

  virtual std::optional<std::vector<char>>
  load(std::filesystem::path const &path,
       std::string const &internalDataName) = 0;

  virtual bool
  save(std::filesystem::path const &path,
       std::vector<std::pair<std::string, std::vector<char>>> const &data) = 0;

  virtual ~IProfileFileParser() = default;
};
