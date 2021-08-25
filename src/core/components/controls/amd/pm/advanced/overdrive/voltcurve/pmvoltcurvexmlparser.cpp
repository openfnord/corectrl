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
#include "pmvoltcurvexmlparser.h"

#include "../pmoverdrive.h"
#include "core/profilepartxmlparserprovider.h"
#include "pmvoltcurve.h"
#include <algorithm>
#include <memory>

class AMD::PMVoltCurveXMLParser::Initializer final
: public AMD::PMVoltCurveProfilePart::Exporter
{
 public:
  Initializer(AMD::PMVoltCurveXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMVoltCurveMode(std::string const &mode) override;
  void takePMVoltCurvePoints(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &points) override;

 private:
  AMD::PMVoltCurveXMLParser &outer_;
};

void AMD::PMVoltCurveXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMVoltCurveXMLParser::Initializer::takePMVoltCurveMode(
    std::string const &mode)
{
  outer_.mode_ = outer_.modeDefault_ = mode;
}

void AMD::PMVoltCurveXMLParser::Initializer::takePMVoltCurvePoints(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &points)
{
  outer_.points_ = outer_.pointsDefault_ = points;
}

AMD::PMVoltCurveXMLParser::PMVoltCurveXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMVoltCurve::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMVoltCurveXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMVoltCurveXMLParser::initializer()
{
  return std::make_unique<AMD::PMVoltCurveXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMVoltCurveXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMVoltCurveXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMVoltCurveXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMVoltCurveXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMVoltCurveXMLParser::takePMVoltCurveMode(std::string const &mode)
{
  mode_ = mode;
}

std::string const &AMD::PMVoltCurveXMLParser::providePMVoltCurveMode() const
{
  return mode_;
}

void AMD::PMVoltCurveXMLParser::takePMVoltCurvePoints(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &points)
{
  points_ = points;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMVoltCurveXMLParser::providePMVoltCurvePoint(unsigned int index) const
{
  if (index < points_.size())
    return points_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMVoltCurveXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto node = parentNode.append_child(ID().c_str());

  node.append_attribute("active") = active_;
  node.append_attribute("mode") = mode_.data();
  savePoints(node);
}

void AMD::PMVoltCurveXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  mode_ = modeDefault_;
  points_ = pointsDefault_;
}

void AMD::PMVoltCurveXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto legacyNode = parentNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == AMD::PMOverdrive::LegacyFVVoltCurveItemID;
  });
  if (!legacyNode) {
    auto node = parentNode.find_child(
        [&](pugi::xml_node const &node) { return node.name() == ID(); });

    active_ = node.attribute("active").as_bool(activeDefault_);
    mode_ = node.attribute("mode").as_string(modeDefault_.data());
    loadPoints(node);
  }
  else {
    active_ = legacyNode.attribute("active").as_bool(activeDefault_);
    mode_ = legacyNode.attribute(LegacyModeAttribute.data())
                .as_string(modeDefault_.data());
    loadPointsFromLegacyNode(legacyNode);
  }
}

void AMD::PMVoltCurveXMLParser::savePoints(pugi::xml_node &node) const
{
  for (auto [freq, volt] : points_) {
    auto pointNode = node.append_child(PointNodeName.data());
    pointNode.append_attribute("freq") = freq.to<unsigned int>();
    pointNode.append_attribute("volt") = volt.to<unsigned int>();
  }
}

void AMD::PMVoltCurveXMLParser::loadPoints(pugi::xml_node &node)
{
  if (!node)
    points_ = pointsDefault_;
  else {
    points_.clear();

    for (auto pointNode : node.children(PointNodeName.data())) {
      auto freqAttr = pointNode.attribute("freq");
      auto voltAttr = pointNode.attribute("volt");

      if (freqAttr && voltAttr) {
        auto freq = freqAttr.as_uint();
        auto volt = voltAttr.as_uint();
        points_.emplace_back(units::frequency::megahertz_t(freq),
                             units::voltage::millivolt_t(volt));
      }
      else // malformed data
        break;
    }

    if (points_.size() != pointsDefault_.size())
      points_ = pointsDefault_;
  }
}

void AMD::PMVoltCurveXMLParser::loadPointsFromLegacyNode(pugi::xml_node &node)
{
  auto pointsNode = node.find_child([&](pugi::xml_node const &node) {
    return node.name() == LegacyPointsNodeName;
  });
  loadPoints(pointsNode);
}

bool const AMD::PMVoltCurveXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMVoltCurve::ItemID,
        []() { return std::make_unique<AMD::PMVoltCurveXMLParser>(); });
