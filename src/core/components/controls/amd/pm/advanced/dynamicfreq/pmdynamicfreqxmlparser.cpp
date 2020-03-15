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
#include "pmdynamicfreqxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmdynamicfreq.h"
#include <memory>

class AMD::PMDynamicFreqXMLParser::Initializer final
: public AMD::PMDynamicFreqProfilePart::Exporter
{
 public:
  Initializer(AMD::PMDynamicFreqXMLParser &outer) noexcept
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
  AMD::PMDynamicFreqXMLParser &outer_;
};

void AMD::PMDynamicFreqXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

AMD::PMDynamicFreqXMLParser::PMDynamicFreqXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMDynamicFreq::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMDynamicFreqXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMDynamicFreqXMLParser::initializer()
{
  return std::make_unique<AMD::PMDynamicFreqXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMDynamicFreqXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMDynamicFreqXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMDynamicFreqXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMDynamicFreqXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMDynamicFreqXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(id_.c_str());
  pmFrequencyNode.append_attribute("active") = active_;
}

void AMD::PMDynamicFreqXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void AMD::PMDynamicFreqXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
}

bool const AMD::PMDynamicFreqXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMDynamicFreq::ItemID,
        []() { return std::make_unique<AMD::PMDynamicFreqXMLParser>(); });
