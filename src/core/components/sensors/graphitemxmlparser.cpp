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
  outer_.color_ = outer_.colorDefault_ = color;
}

GraphItemXMLParser::GraphItemXMLParser(std::string_view id) noexcept
: ProfilePartXMLParser(*this, *this)
, id_(id)
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

void GraphItemXMLParser::takeColor(const std::string &color)
{
  color_ = color;
}

std::string const &GraphItemXMLParser::provideColor() const
{
  return color_;
}

void GraphItemXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto itemNode = parentNode.append_child(id_.c_str());
  itemNode.append_attribute("active") = active_;
  itemNode.append_attribute("color") = color_.c_str();
}

void GraphItemXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  color_ = colorDefault_;
}

void GraphItemXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto itemNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = itemNode.attribute("active").as_bool(activeDefault_);
  color_ = itemNode.attribute("color").as_string(colorDefault_.c_str());
}
