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
#include "pmpowercapxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmpowercap.h"
#include <memory>

class AMD::PMPowerCapXMLParser::Initializer final
: public AMD::PMPowerCapProfilePart::Exporter
{
 public:
  Initializer(AMD::PMPowerCapXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMPowerCapValue(units::power::watt_t value) override;

 private:
  AMD::PMPowerCapXMLParser &outer_;
};

void AMD::PMPowerCapXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMPowerCapXMLParser::Initializer::takePMPowerCapValue(
    units::power::watt_t value)
{
  outer_.value_ = outer_.valueDefault_ = value;
}

AMD::PMPowerCapXMLParser::PMPowerCapXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMPowerCap::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMPowerCapXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerCapXMLParser::initializer()
{
  return std::make_unique<AMD::PMPowerCapXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMPowerCapXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerCapXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMPowerCapXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMPowerCapXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMPowerCapXMLParser::takePMPowerCapValue(units::power::watt_t value)
{
  value_ = value;
}

units::power::watt_t AMD::PMPowerCapXMLParser::providePMPowerCapValue() const
{
  return value_;
}

void AMD::PMPowerCapXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(id_.c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("value") = value_.to<unsigned int>();
}

void AMD::PMPowerCapXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  value_ = valueDefault_;
}

void AMD::PMPowerCapXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  value_ = units::power::watt_t(
      pmFixedNode.attribute("value").as_uint(valueDefault_.to<unsigned int>()));
}

bool const AMD::PMPowerCapXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMPowerCap::ItemID, []() {
      return std::make_unique<AMD::PMPowerCapXMLParser>();
    });
