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
#include "fanfixedxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "fanfixed.h"
#include <memory>

class AMD::FanFixedXMLParser::Initializer final
: public AMD::FanFixedProfilePart::Exporter
{
 public:
  Initializer(AMD::FanFixedXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takeFanFixedValue(units::concentration::percent_t value) override;
  void takeFanFixedFanStop(bool enabled) override;
  void takeFanFixedFanStartValue(units::concentration::percent_t value) override;

 private:
  AMD::FanFixedXMLParser &outer_;
};

void AMD::FanFixedXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::FanFixedXMLParser::Initializer::takeFanFixedValue(
    units::concentration::percent_t value)
{
  outer_.value_ = outer_.valueDefault_ = value * 100;
}

void AMD::FanFixedXMLParser::Initializer::takeFanFixedFanStop(bool enabled)
{
  outer_.fanStop_ = outer_.fanStopDefault_ = enabled;
}

void AMD::FanFixedXMLParser::Initializer::takeFanFixedFanStartValue(
    units::concentration::percent_t value)
{
  outer_.fanStartValue_ = outer_.fanStartValueDefault_ = value * 100;
}

AMD::FanFixedXMLParser::FanFixedXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::FanFixed::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::FanFixedXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::FanFixedXMLParser::initializer()
{
  return std::make_unique<AMD::FanFixedXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanFixedXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanFixedXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::FanFixedXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::FanFixedXMLParser::provideActive() const
{
  return active_;
}

void AMD::FanFixedXMLParser::takeFanFixedValue(units::concentration::percent_t value)
{
  value_ = value * 100;
}

units::concentration::percent_t AMD::FanFixedXMLParser::provideFanFixedValue() const
{
  return value_;
}

void AMD::FanFixedXMLParser::takeFanFixedFanStop(bool enabled)
{
  fanStop_ = enabled;
}

bool AMD::FanFixedXMLParser::provideFanFixedFanStop() const
{
  return fanStop_;
}

void AMD::FanFixedXMLParser::takeFanFixedFanStartValue(
    units::concentration::percent_t value)
{
  fanStartValue_ = value * 100;
}

units::concentration::percent_t
AMD::FanFixedXMLParser::provideFanFixedFanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanFixedXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(id_.c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("value") = value_;
  pmFixedNode.append_attribute("fanStop") = fanStop_;
  pmFixedNode.append_attribute("fanStartValue") = fanStartValue_;
}

void AMD::FanFixedXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  value_ = valueDefault_;
  fanStop_ = fanStopDefault_;
  fanStartValue_ = fanStartValueDefault_;
}

void AMD::FanFixedXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  value_ = pmFixedNode.attribute("value").as_int(valueDefault_);
  fanStop_ = pmFixedNode.attribute("fanStop").as_bool(fanStopDefault_);
  fanStartValue_ =
      pmFixedNode.attribute("fanStartValue").as_int(fanStartValueDefault_);
}

bool const AMD::FanFixedXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::FanFixed::ItemID, []() {
      return std::make_unique<AMD::FanFixedXMLParser>();
    });
