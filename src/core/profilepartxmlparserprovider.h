// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofilepartxmlparserprovider.h"
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class IProfilePartXMLParser;

class ProfilePartXMLParserProvider final : public IProfilePartXMLParserProvider
{
 public:
  std::unordered_map<std::string,
                     std::function<std::unique_ptr<IProfilePartXMLParser>()>> const &
  profilePartParserProviders() const override;

  static bool registerProvider(
      std::string_view componentID,
      std::function<std::unique_ptr<IProfilePartXMLParser>()> &&provider);

 private:
  static std::unordered_map<
      std::string, std::function<std::unique_ptr<IProfilePartXMLParser>()>> &
  profilePartParserProviders_();
};
