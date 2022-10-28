// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmperfmodexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmperfmode.h"
#include <memory>

AMD::PMPerfModeXMLParser::PMPerfModeXMLParser() noexcept
: ControlModeXMLParser(AMD::PMPerfMode::ItemID)
{
}

bool const AMD::PMPerfModeXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMPerfMode::ItemID, []() {
      return std::make_unique<AMD::PMPerfModeXMLParser>();
    });
