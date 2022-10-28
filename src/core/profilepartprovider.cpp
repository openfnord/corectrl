// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilepartprovider.h"

#include <utility>

std::unordered_map<std::string, std::function<std::unique_ptr<IProfilePart>()>> const &
ProfilePartProvider::profilePartProviders() const
{
  return profilePartProviders_();
}

std::unordered_map<std::string, std::function<std::unique_ptr<IProfilePart>()>> &
ProfilePartProvider::profilePartProviders_()
{
  static std::unordered_map<std::string,
                            std::function<std::unique_ptr<IProfilePart>()>>
      providers;
  return providers;
}

bool ProfilePartProvider::registerProvider(
    std::string_view componentID,
    std::function<std::unique_ptr<IProfilePart>()> &&provider)
{
  profilePartProviders_().emplace(std::string(componentID), std::move(provider));
  return true;
}
