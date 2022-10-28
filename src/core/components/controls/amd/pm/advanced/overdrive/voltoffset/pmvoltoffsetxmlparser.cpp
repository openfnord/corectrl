// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmvoltoffsetxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmvoltoffset.h"
#include <memory>

class AMD::PMVoltOffsetXMLParser::Initializer final
: public AMD::PMVoltOffsetProfilePart::Exporter
{
 public:
  Initializer(AMD::PMVoltOffsetXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMVoltOffsetValue(units::voltage::millivolt_t value) override;

 private:
  AMD::PMVoltOffsetXMLParser &outer_;
};

void AMD::PMVoltOffsetXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMVoltOffsetXMLParser::Initializer::takePMVoltOffsetValue(
    units::voltage::millivolt_t value)
{
  outer_.value_ = outer_.valueDefault_ = value;
}

AMD::PMVoltOffsetXMLParser::PMVoltOffsetXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMVoltOffset::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMVoltOffsetXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMVoltOffsetXMLParser::initializer()
{
  return std::make_unique<AMD::PMVoltOffsetXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMVoltOffsetXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMVoltOffsetXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMVoltOffsetXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMVoltOffsetXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMVoltOffsetXMLParser::takePMVoltOffsetValue(
    units::voltage::millivolt_t value)
{
  value_ = value;
}

units::voltage::millivolt_t
AMD::PMVoltOffsetXMLParser::providePMVoltOffsetValue() const
{
  return value_;
}

void AMD::PMVoltOffsetXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(ID().c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("value") = value_.to<int>();
}

void AMD::PMVoltOffsetXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  value_ = valueDefault_;
}

void AMD::PMVoltOffsetXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  value_ = units::voltage::millivolt_t(
      pmFixedNode.attribute("value").as_int(valueDefault_.to<int>()));
}

bool const AMD::PMVoltOffsetXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMVoltOffset::ItemID,
        []() { return std::make_unique<AMD::PMVoltOffsetXMLParser>(); });
