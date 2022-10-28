// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqmodexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfreqmode.h"
#include <memory>

AMD::PMFreqModeXMLParser::PMFreqModeXMLParser() noexcept
: ControlModeXMLParser(AMD::PMFreqMode::ItemID)
{
}

bool const AMD::PMFreqModeXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMFreqMode::ItemID, []() {
      return std::make_unique<AMD::PMFreqModeXMLParser>();
    });
