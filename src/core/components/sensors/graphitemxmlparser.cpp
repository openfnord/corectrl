// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "graphitemxmlparser.h"

#include <memory>

class GraphItemXMLParser::Initializer final
: public GraphItemProfilePart::Exporter
{
 public:
  Initializer(GraphItemXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takeColor(std::string const &color) override;

 private:
  GraphItemXMLParser &outer_;
};

void GraphItemXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void GraphItemXMLParser::Initializer::takeColor(std::string const &color)
{
  outer_.colorDefault_ = color;
}

GraphItemXMLParser::GraphItemXMLParser(std::string_view id) noexcept
: ProfilePartXMLParser(id, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
GraphItemXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> GraphItemXMLParser::initializer()
{
  return std::make_unique<GraphItemXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
GraphItemXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
GraphItemXMLParser::provideImporter(Item const &)
{
  return {};
}

void GraphItemXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool GraphItemXMLParser::provideActive() const
{
  return active_;
}

void GraphItemXMLParser::takeColor(const std::string &)
{
}

std::string const &GraphItemXMLParser::provideColor() const
{
  return colorDefault_;
}

void GraphItemXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto itemNode = parentNode.append_child(ID().c_str());
  itemNode.append_attribute("active") = active_;
}

void GraphItemXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void GraphItemXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto itemNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = itemNode.attribute("active").as_bool(activeDefault_);
}
