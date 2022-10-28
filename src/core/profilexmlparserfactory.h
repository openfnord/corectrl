// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>

class IProfile;
class IProfileParser;
class IProfilePartXMLParserProvider;

class ProfileXMLParserFactory
{
 public:
  ProfileXMLParserFactory(std::unique_ptr<IProfilePartXMLParserProvider>
                              &&profilePartParserProvider) noexcept;

  std::unique_ptr<IProfileParser> build(IProfile const &baseProfile) const;

 private:
  std::unique_ptr<IProfilePartXMLParserProvider> profilePartParserProvider_;
};
