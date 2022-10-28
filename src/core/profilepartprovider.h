// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofilepartprovider.h"
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class IProfilePart;

class ProfilePartProvider final : public IProfilePartProvider
{
 public:
  std::unordered_map<std::string, std::function<std::unique_ptr<IProfilePart>()>> const &
  profilePartProviders() const override;

  static bool
  registerProvider(std::string_view componentID,
                   std::function<std::unique_ptr<IProfilePart>()> &&provider);

 private:
  static std::unordered_map<std::string,
                            std::function<std::unique_ptr<IProfilePart>()>> &
  profilePartProviders_();
};
