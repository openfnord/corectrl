// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
    Item const &, std::unique_ptr<IProfilePartXMLParser> &&part)
{
  outer_.parsers_.emplace_back(std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupXMLParser::Factory::provideExporter(Item const &i)
{
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
  for (auto &parser : outer_.parsers_) {
    if (parser->ID() == i.ID() && (parser->instanceID() == i.instanceID() ||
                                   parser->instanceID().empty())) {

      // Each item instance must have its own initializer
      auto const id = i.ID() != i.instanceID() ? i.ID() + i.instanceID()
                                               : i.ID();
      if (initializers_.count(id) > 0)
        return *initializers_.at(id);
      else {
        auto initializer = parser->initializer();
        if (initializer != nullptr) {
          initializers_.emplace(id, std::move(initializer));
          return *initializers_.at(id);
        }
      }

      break;
    }
  }

  return {};
}

void ControlGroupXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

ControlGroupXMLParser::ControlGroupXMLParser(std::string_view id) noexcept
: ProfilePartXMLParser(id, *this, *this)
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
  auto parserIt = std::find_if(
      parsers_.cbegin(), parsers_.cend(), [&](auto &part) {
        return part->ID() == i.ID() && part->instanceID() == i.instanceID();
      });

  if (parserIt != parsers_.cend())
    return (*parserIt)->profilePartExporter();

  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlGroupXMLParser::provideImporter(Item const &i)
{
  auto parserIt = std::find_if(
      parsers_.cbegin(), parsers_.cend(), [&](auto &part) {
        return part->ID() == i.ID() && part->instanceID() == i.instanceID();
      });

  if (parserIt != parsers_.cend())
    return (*parserIt)->profilePartImporter();

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
  auto node = parentNode.append_child(ID().c_str());
  node.append_attribute("active") = active_;

  for (auto &parser : parsers_)
    parser->appendTo(node);
}

void ControlGroupXMLParser::resetAttributes()
{
  active_ = activeDefault();
}

void ControlGroupXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto node = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = node.attribute("active").as_bool(activeDefault());
  loadComponents(node);
}

void ControlGroupXMLParser::loadComponents(pugi::xml_node const &parentNode)
{
  for (auto &parser : parsers_)
    parser->loadFrom(parentNode);
}

bool ControlGroupXMLParser::activeDefault() const
{
  return activeDefault_;
}
