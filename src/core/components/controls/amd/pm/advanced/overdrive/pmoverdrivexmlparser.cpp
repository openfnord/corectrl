// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverdrivexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmoverdrive.h"
#include <memory>

AMD::PMOverdriveXMLParser::PMOverdriveXMLParser() noexcept
: ControlGroupXMLParser(AMD::PMOverdrive::ItemID)
{
}

void AMD::PMOverdriveXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto node = parentNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == AMD::PMOverdrive::ItemID;
  });

  takeActive(node.attribute("active").as_bool(activeDefault()));

  if (!node) {
    // Legacy control settings section might be present in the profile.
    // These section hangs from the parent node, so we must pass it
    // to load the settings on each sub-component.
    node = parentNode;
  }

  loadComponents(node);
}

bool const AMD::PMOverdriveXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMOverdrive::ItemID,
        []() { return std::make_unique<AMD::PMOverdriveXMLParser>(); });
