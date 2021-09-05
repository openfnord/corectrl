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
#include "noopxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "noop.h"
#include <memory>

class NoopXMLParser::Initializer final : public NoopProfilePart::Exporter
{
 public:
  Initializer(NoopXMLParser &outer) noexcept
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
  NoopXMLParser &outer_;
};

void NoopXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

NoopXMLParser::NoopXMLParser() noexcept
: ProfilePartXMLParser(Noop::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
NoopXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> NoopXMLParser::initializer()
{
  return std::make_unique<NoopXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
NoopXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
NoopXMLParser::provideImporter(Item const &)
{
  return {};
}

void NoopXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool NoopXMLParser::provideActive() const
{
  return active_;
}

void NoopXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(ID().c_str());
  pmFixedNode.append_attribute("active") = active_;
}

void NoopXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void NoopXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
}

bool const NoopXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        Noop::ItemID, []() { return std::make_unique<NoopXMLParser>(); });
