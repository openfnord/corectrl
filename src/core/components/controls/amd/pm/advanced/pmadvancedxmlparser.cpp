// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmadvancedxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmadvanced.h"
#include <memory>

AMD::PMAdvancedXMLParser::PMAdvancedXMLParser() noexcept
: ControlGroupXMLParser(AMD::PMAdvanced::ItemID)
{
}

bool const AMD::PMAdvancedXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMAdvanced::ItemID, []() {
      return std::make_unique<AMD::PMAdvancedXMLParser>();
    });
