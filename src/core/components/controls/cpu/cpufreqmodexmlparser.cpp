// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpufreqmodexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "cpufreqmode.h"
#include <memory>

CPUFreqModeXMLParser::CPUFreqModeXMLParser() noexcept
: ControlModeXMLParser(CPUFreqMode::ItemID)
{
}

void CPUFreqModeXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto node = parentNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == CPUFreqMode::ItemID;
  });

  takeActive(node.attribute("active").as_bool(activeDefault()));
  takeMode(node.attribute("mode").as_string(modeDefault().c_str()));

  if (!node) {
    // Legacy control settings might be present in the profile.
    // The old control settings were attach to this node parent,
    // so we must pass it in order to load its settings.
    node = parentNode;
  }

  loadComponents(node);
}

bool const CPUFreqModeXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(CPUFreqMode::ItemID, []() {
      return std::make_unique<CPUFreqModeXMLParser>();
    });
