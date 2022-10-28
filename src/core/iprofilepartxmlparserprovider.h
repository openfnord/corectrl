// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class IProfilePartXMLParser;

class IProfilePartXMLParserProvider
{
 public:
  virtual std::unordered_map<
      std::string, std::function<std::unique_ptr<IProfilePartXMLParser>()>> const &
  profilePartParserProviders() const = 0;

  virtual ~IProfilePartXMLParserProvider() = default;
};
