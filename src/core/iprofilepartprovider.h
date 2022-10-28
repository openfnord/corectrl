// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class IProfilePart;

class IProfilePartProvider
{
 public:
  virtual std::unordered_map<std::string,
                             std::function<std::unique_ptr<IProfilePart>()>> const &
  profilePartProviders() const = 0;

  virtual ~IProfilePartProvider() = default;
};
