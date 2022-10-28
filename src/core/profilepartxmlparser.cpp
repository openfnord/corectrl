// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilepartxmlparser.h"

#include "iprofilepartxmlparserprovider.h"
#include "item.h"
#include <utility>

ProfilePartXMLParser::ProfilePartXMLParser(std::string_view id,
                                           Importable::Importer &importer,
                                           Exportable::Exporter &exporter) noexcept
: id_(id)
, importer_(importer)
, exporter_(exporter)
{
}

std::string const &ProfilePartXMLParser::ID() const
{
  return id_;
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
