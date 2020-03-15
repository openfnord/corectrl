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
#include "pmfreqodxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfreqod.h"
#include <memory>

class AMD::PMFreqOdXMLParser::Initializer final
: public AMD::PMFreqOdProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFreqOdXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqOdSclkOd(unsigned int value) override;
  void takePMFreqOdMclkOd(unsigned int value) override;

 private:
  AMD::PMFreqOdXMLParser &outer_;
};

void AMD::PMFreqOdXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFreqOdXMLParser::Initializer::takePMFreqOdSclkOd(unsigned int value)
{
  outer_.sclkOd_ = outer_.sclkOdDefault_ = value;
}

void AMD::PMFreqOdXMLParser::Initializer::takePMFreqOdMclkOd(unsigned int value)
{
  outer_.mclkOd_ = outer_.mclkOdDefault_ = value;
}

AMD::PMFreqOdXMLParser::PMFreqOdXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMFreqOd::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqOdXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqOdXMLParser::initializer()
{
  return std::make_unique<AMD::PMFreqOdXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqOdXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqOdXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFreqOdXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFreqOdXMLParser::provideActive() const
{
  return active_;
}

unsigned int AMD::PMFreqOdXMLParser::providePMFreqOdSclkOd() const
{
  return sclkOd_;
}

void AMD::PMFreqOdXMLParser::takePMFreqOdSclkOd(unsigned int value)
{
  sclkOd_ = value;
}

unsigned int AMD::PMFreqOdXMLParser::providePMFreqOdMclkOd() const
{
  return mclkOd_;
}

void AMD::PMFreqOdXMLParser::takePMFreqOdMclkOd(unsigned int value)
{
  mclkOd_ = value;
}

void AMD::PMFreqOdXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(id_.c_str());
  pmFrequencyNode.append_attribute("active") = active_;
  pmFrequencyNode.append_attribute("sclkOd") = sclkOd_;
  pmFrequencyNode.append_attribute("mclkOd") = mclkOd_;
}

void AMD::PMFreqOdXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  sclkOd_ = sclkOdDefault_;
  mclkOd_ = mclkOdDefault_;
}

void AMD::PMFreqOdXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
  sclkOd_ = pmFrequencyNode.attribute("sclkOd").as_uint(sclkOdDefault_);
  mclkOd_ = pmFrequencyNode.attribute("mclkOd").as_uint(mclkOdDefault_);
}

bool const AMD::PMFreqOdXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMFreqOd::ItemID, []() {
      return std::make_unique<AMD::PMFreqOdXMLParser>();
    });
