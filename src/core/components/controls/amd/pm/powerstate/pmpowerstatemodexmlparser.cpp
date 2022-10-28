// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerstatemodexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmpowerstatemode.h"
#include <memory>

AMD::PMPowerStateModeXMLParser::PMPowerStateModeXMLParser() noexcept
: ControlModeXMLParser(AMD::PMPowerStateMode::ItemID)
{
}

void AMD::PMPowerStateModeXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto node = parentNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == AMD::PMPowerStateMode::ItemID;
  });

  takeActive(node.attribute("active").as_bool(activeDefault()));
  takeActive(node.attribute("mode").as_string(modeDefault().c_str()));

  if (!node) {
    // Legacy control settings might be present in the profile.
    // The old control settings were attach to this node parent,
    // so we must pass it in order to load its settings.
    node = parentNode;
  }

  loadComponents(node);
}

bool const AMD::PMPowerStateModeXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMPowerStateMode::ItemID,
        []() { return std::make_unique<AMD::PMPowerStateModeXMLParser>(); });
