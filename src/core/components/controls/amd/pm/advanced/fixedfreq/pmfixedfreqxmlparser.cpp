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
#include "pmfixedfreqxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfixedfreq.h"
#include <memory>

class AMD::PMFixedFreqXMLParser::Initializer final
: public AMD::PMFixedFreqProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFixedFreqXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFixedFreqSclkIndex(unsigned int index) override;
  void takePMFixedFreqMclkIndex(unsigned int index) override;

 private:
  AMD::PMFixedFreqXMLParser &outer_;
};

void AMD::PMFixedFreqXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFixedFreqXMLParser::Initializer::takePMFixedFreqSclkIndex(
    unsigned int index)
{
  outer_.sclkIndex_ = outer_.sclkIndexDefault_ = index;
}

void AMD::PMFixedFreqXMLParser::Initializer::takePMFixedFreqMclkIndex(
    unsigned int index)
{
  outer_.mclkIndex_ = outer_.mclkIndexDefault_ = index;
}

AMD::PMFixedFreqXMLParser::PMFixedFreqXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMFixedFreq::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFixedFreqXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFixedFreqXMLParser::initializer()
{
  return std::make_unique<AMD::PMFixedFreqXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFixedFreqXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedFreqXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFixedFreqXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFixedFreqXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFixedFreqXMLParser::takePMFixedFreqSclkIndex(unsigned int index)
{
  sclkIndex_ = index;
}

unsigned int AMD::PMFixedFreqXMLParser::providePMFixedFreqSclkIndex() const
{
  return sclkIndex_;
}

void AMD::PMFixedFreqXMLParser::takePMFixedFreqMclkIndex(unsigned int index)
{
  mclkIndex_ = index;
}

unsigned int AMD::PMFixedFreqXMLParser::providePMFixedFreqMclkIndex() const
{
  return mclkIndex_;
}

void AMD::PMFixedFreqXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(id_.c_str());
  pmFrequencyNode.append_attribute("active") = active_;
  pmFrequencyNode.append_attribute("sclkState") = sclkIndex_;
  pmFrequencyNode.append_attribute("mclkState") = mclkIndex_;
}

void AMD::PMFixedFreqXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  sclkIndex_ = sclkIndexDefault_;
  mclkIndex_ = mclkIndexDefault_;
}

void AMD::PMFixedFreqXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
  sclkIndex_ = pmFrequencyNode.attribute("sclkState").as_uint(sclkIndexDefault_);
  mclkIndex_ = pmFrequencyNode.attribute("mclkState").as_uint(mclkIndexDefault_);
}

bool const AMD::PMFixedFreqXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMFixedFreq::ItemID,
        []() { return std::make_unique<AMD::PMFixedFreqXMLParser>(); });
