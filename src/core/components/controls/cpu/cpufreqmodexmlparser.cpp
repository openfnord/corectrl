//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
