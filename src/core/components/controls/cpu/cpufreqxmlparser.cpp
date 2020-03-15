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
#include "cpufreqxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "cpufreq.h"
#include <memory>

class CPUFreqXMLParser::Initializer final : public CPUFreqProfilePart::Exporter
{
 public:
  Initializer(CPUFreqXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takeCPUFreqScalingGovernor(std::string const &governor) override;

 private:
  CPUFreqXMLParser &outer_;
};

void CPUFreqXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void CPUFreqXMLParser::Initializer::takeCPUFreqScalingGovernor(
    std::string const &governor)
{
  outer_.scalingGovernor_ = outer_.scalingGovernorDefault_ = governor;
}

CPUFreqXMLParser::CPUFreqXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(CPUFreq::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
CPUFreqXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> CPUFreqXMLParser::initializer()
{
  return std::make_unique<CPUFreqXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUFreqXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
CPUFreqXMLParser::provideImporter(Item const &)
{
  return {};
}

void CPUFreqXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool CPUFreqXMLParser::provideActive() const
{
  return active_;
}

void CPUFreqXMLParser::takeCPUFreqScalingGovernor(std::string const &governor)
{
  scalingGovernor_ = governor;
}

std::string const &CPUFreqXMLParser::provideCPUFreqScalingGovernor() const
{
  return scalingGovernor_;
}

void CPUFreqXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(id_.c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("scalingGovernor") = scalingGovernor_.c_str();
}

void CPUFreqXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  scalingGovernor_ = scalingGovernorDefault_;
}

void CPUFreqXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  scalingGovernor_ = pmFixedNode.attribute("scalingGovernor")
                         .as_string(scalingGovernorDefault_.c_str());
}

bool const CPUFreqXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        CPUFreq::ItemID, []() { return std::make_unique<CPUFreqXMLParser>(); });
