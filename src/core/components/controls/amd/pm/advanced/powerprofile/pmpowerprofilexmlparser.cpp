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
#include "pmpowerprofilexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmpowerprofile.h"
#include <memory>

class AMD::PMPowerProfileXMLParser::Initializer final
: public AMD::PMPowerProfileProfilePart::Exporter
{
 public:
  Initializer(AMD::PMPowerProfileXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMPowerProfileMode(std::string const &mode) override;

 private:
  AMD::PMPowerProfileXMLParser &outer_;
};

void AMD::PMPowerProfileXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMPowerProfileXMLParser::Initializer::takePMPowerProfileMode(
    std::string const &mode)
{
  outer_.mode_ = outer_.modeDefault_ = mode;
}

AMD::PMPowerProfileXMLParser::PMPowerProfileXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMPowerProfile::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMPowerProfileXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerProfileXMLParser::initializer()
{
  return std::make_unique<AMD::PMPowerProfileXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMPowerProfileXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerProfileXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMPowerProfileXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMPowerProfileXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMPowerProfileXMLParser::takePMPowerProfileMode(std::string const &mode)
{
  mode_ = mode;
}

std::string const &AMD::PMPowerProfileXMLParser::providePMPowerProfileMode() const
{
  return mode_;
}

void AMD::PMPowerProfileXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(id_.c_str());
  pmFrequencyNode.append_attribute("active") = active_;
  pmFrequencyNode.append_attribute("mode") = mode_.c_str();
}

void AMD::PMPowerProfileXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  mode_ = modeDefault_;
}

void AMD::PMPowerProfileXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
  mode_ = pmFrequencyNode.attribute("mode").as_string(modeDefault_.c_str());
}

bool const AMD::PMPowerProfileXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMPowerProfile::ItemID,
        []() { return std::make_unique<AMD::PMPowerProfileXMLParser>(); });
