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
#include "controlgroupxmlparser.h"

#include "pugixml/pugixml.hpp"
#include <utility>

class ControlGroupXMLParser::Factory final
: public ProfilePartXMLParser::Factory
, public ControlGroupProfilePart::Exporter
{
 public:
  Factory(IProfilePartXMLParserProvider const &profilePartParserProvider,
          ControlGroupXMLParser &outer) noexcept
  : ProfilePartXMLParser::Factory(profilePartParserProvider)
  , outer_(outer)
  {
  }

  void takePartParser(Item const &i,
                      std::unique_ptr<IProfilePartXMLParser> &&part) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override
  {
  }

 private:
  ControlGroupXMLParser &outer_;
};

void ControlGroupXMLParser::Factory::takePartParser(
    Item const &i, std::unique_ptr<IProfilePartXMLParser> &&part)
{
  outer_.parsers_.emplace(i.ID(), std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupXMLParser::Factory::provideExporter(Item const &i)
{
  if (i.ID() == outer_.id_)
    return *this;
  else
    return factory(i);
}

class ControlGroupXMLParser::Initializer final
: public ControlGroupProfilePart::Exporter
{
 public:
  Initializer(ControlGroupXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;

 private:
  ControlGroupXMLParser &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupXMLParser::Initializer::provideExporter(Item const &i)
{
  auto &id = i.ID();
  if (outer_.parsers_.count(id) > 0) {
    if (initializers_.count(id) > 0)
      return *initializers_.at(id);
    else {
      auto initializer = outer_.parsers_.at(id)->initializer();
      if (initializer != nullptr) {
        initializers_.emplace(id, std::move(initializer));
        return *initializers_.at(id);
      }
    }
  }

  return {};
}

void ControlGroupXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

ControlGroupXMLParser::ControlGroupXMLParser(std::string_view id) noexcept
: ProfilePartXMLParser(*this, *this)
, id_(id)
{
}

std::unique_ptr<Exportable::Exporter> ControlGroupXMLParser::factory(
    IProfilePartXMLParserProvider const &profilePartXMLParserProvider)
{
  return std::make_unique<ControlGroupXMLParser::Factory>(
      profilePartXMLParserProvider, *this);
}

std::unique_ptr<Exportable::Exporter> ControlGroupXMLParser::initializer()
{
  return std::make_unique<ControlGroupXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupXMLParser::provideExporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartExporter();

  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlGroupXMLParser::provideImporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartImporter();

  return {};
}

void ControlGroupXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool ControlGroupXMLParser::provideActive() const
{
  return active_;
}

void ControlGroupXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto node = parentNode.append_child(id_.c_str());
  node.append_attribute("active") = active_;

  for (auto &[key, component] : parsers_)
    component->appendTo(node);
}

void ControlGroupXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void ControlGroupXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto node = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = node.attribute("active").as_bool(activeDefault_);

  for (auto &[key, component] : parsers_)
    component->loadFrom(node);
}
