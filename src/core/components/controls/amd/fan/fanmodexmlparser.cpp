// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanmodexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "fanmode.h"
#include <memory>

AMD::FanModeXMLParser::FanModeXMLParser() noexcept
: ControlModeXMLParser(AMD::FanMode::ItemID)
{
}

bool const AMD::FanModeXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::FanMode::ItemID, []() {
      return std::make_unique<AMD::FanModeXMLParser>();
    });
