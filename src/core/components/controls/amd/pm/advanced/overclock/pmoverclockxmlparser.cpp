// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverclockxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmoverclock.h"
#include <memory>

AMD::PMOverclockXMLParser::PMOverclockXMLParser() noexcept
: ControlGroupXMLParser(AMD::PMOverclock::ItemID)
{
}

bool const AMD::PMOverclockXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMOverclock::ItemID,
        []() { return std::make_unique<AMD::PMOverclockXMLParser>(); });
