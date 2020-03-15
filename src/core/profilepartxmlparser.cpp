//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "profilepartxmlparser.h"

#include "iprofilepartxmlparserprovider.h"
#include "item.h"
#include <utility>

ProfilePartXMLParser::ProfilePartXMLParser(Importable::Importer &importer,
                                           Exportable::Exporter &exporter) noexcept
: importer_(importer)
, exporter_(exporter)
{
}

void ProfilePartXMLParser::loadFrom(pugi::xml_node const &parentNode)
{
  resetAttributes();
  loadPartFrom(parentNode);
}

Importable::Importer &ProfilePartXMLParser::profilePartImporter() const
{
  return importer_;
}

Exportable::Exporter &ProfilePartXMLParser::profilePartExporter() const
{
  return exporter_;
}

ProfilePartXMLParser::Factory::Factory(
    IProfilePartXMLParserProvider const &profilePartParserProvider) noexcept
: profilePartParserProvider_(profilePartParserProvider)
{
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ProfilePartXMLParser::Factory::factory(Item const &i)
{
  auto partParser = createPartParser(i.ID());
  if (partParser != nullptr) {
    auto factory = partParser->factory(profilePartParserProvider_);
    takePartParser(i, std::move(partParser));

    if (factory != nullptr) {
      factories_.emplace_back(std::move(factory));
      return *factories_.back();
    }
  }

  return {};
}

std::unique_ptr<IProfilePartXMLParser>
ProfilePartXMLParser::Factory::createPartParser(std::string const &componentID) const
{
  auto &partParserProviders =
      profilePartParserProvider_.profilePartParserProviders();
  auto const providerIt = partParserProviders.find(componentID);
  if (providerIt != partParserProviders.cend())
    return providerIt->second();

  return nullptr;
}
