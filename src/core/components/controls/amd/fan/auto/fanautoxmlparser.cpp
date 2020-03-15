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
#include "fanautoxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "fanauto.h"
#include <memory>

class AMD::FanAutoXMLParser::Initializer final
: public AMD::FanAutoProfilePart::Exporter
{
 public:
  Initializer(AMD::FanAutoXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;

 private:
  AMD::FanAutoXMLParser &outer_;
};

void AMD::FanAutoXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

AMD::FanAutoXMLParser::FanAutoXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::FanAuto::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::FanAutoXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::FanAutoXMLParser::initializer()
{
  return std::make_unique<AMD::FanAutoXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanAutoXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanAutoXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::FanAutoXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::FanAutoXMLParser::provideActive() const
{
  return active_;
}

void AMD::FanAutoXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(id_.c_str());
  pmFixedNode.append_attribute("active") = active_;
}

void AMD::FanAutoXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void AMD::FanAutoXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
}

bool const AMD::FanAutoXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::FanAuto::ItemID, []() {
      return std::make_unique<AMD::FanAutoXMLParser>();
    });
