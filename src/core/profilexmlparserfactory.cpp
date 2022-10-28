// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilexmlparserfactory.h"

#include "iprofile.h"
#include "iprofileparser.h"
#include "iprofilepartxmlparserprovider.h"
#include "profilexmlparser.h"
#include <utility>

ProfileXMLParserFactory::ProfileXMLParserFactory(
    std::unique_ptr<IProfilePartXMLParserProvider> &&profilePartParserProvider) noexcept
: profilePartParserProvider_(std::move(profilePartParserProvider))
{
}

std::unique_ptr<IProfileParser>
ProfileXMLParserFactory::build(IProfile const &baseProfile) const
{
  auto parser = std::make_unique<ProfileXMLParser>();
  ProfileXMLParser::Factory parserFactory(*profilePartParserProvider_, *parser);
  baseProfile.exportWith(parserFactory);
  return std::move(parser);
}
