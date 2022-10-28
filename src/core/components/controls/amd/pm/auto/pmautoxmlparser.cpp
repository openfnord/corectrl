// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmautoxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmauto.h"
#include <memory>

class AMD::PMAutoXMLParser::Initializer final
: public AMD::PMAutoProfilePart::Exporter
{
 public:
  Initializer(AMD::PMAutoXMLParser &outer) noexcept
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
  AMD::PMAutoXMLParser &outer_;
};

void AMD::PMAutoXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

AMD::PMAutoXMLParser::PMAutoXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMAuto::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMAutoXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMAutoXMLParser::initializer()
{
  return std::make_unique<AMD::PMAutoXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMAutoXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMAutoXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMAutoXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMAutoXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMAutoXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(ID().c_str());
  pmFixedNode.append_attribute("active") = active_;
}

void AMD::PMAutoXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void AMD::PMAutoXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
}

bool const AMD::PMAutoXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMAuto::ItemID, []() {
      return std::make_unique<AMD::PMAutoXMLParser>();
    });
