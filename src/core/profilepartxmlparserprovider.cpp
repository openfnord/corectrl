// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilepartxmlparserprovider.h"

#include <utility>

std::unordered_map<std::string,
                   std::function<std::unique_ptr<IProfilePartXMLParser>()>> const &
ProfilePartXMLParserProvider::profilePartParserProviders() const
{
  return profilePartParserProviders_();
}

std::unordered_map<std::string,
                   std::function<std::unique_ptr<IProfilePartXMLParser>()>> &
ProfilePartXMLParserProvider::profilePartParserProviders_()
{
  static std::unordered_map<
      std::string, std::function<std::unique_ptr<IProfilePartXMLParser>()>>
      providers;
  return providers;
}

bool ProfilePartXMLParserProvider::registerProvider(
    std::string_view componentID,
    std::function<std::unique_ptr<IProfilePartXMLParser>()> &&provider)
{
  profilePartParserProviders_().emplace(std::string(componentID),
                                        std::move(provider));
  return true;
}
