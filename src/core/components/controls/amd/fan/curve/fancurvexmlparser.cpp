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
#include "fancurvexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "fancurve.h"
#include <cmath>
#include <memory>

class AMD::FanCurveXMLParser::Initializer final
: public AMD::FanCurveProfilePart::Exporter
{
 public:
  Initializer(AMD::FanCurveXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void
  takeFanCurvePoints(std::vector<AMD::FanCurve::Point> const &points) override;
  void takeFanCurveFanStop(bool enabled) override;
  void takeFanCurveFanStartValue(units::concentration::percent_t value) override;

 private:
  AMD::FanCurveXMLParser &outer_;
};

void AMD::FanCurveXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::FanCurveXMLParser::Initializer::takeFanCurvePoints(
    std::vector<AMD::FanCurve::Point> const &points)
{
  outer_.points_ = outer_.pointsDefault_ = points;
}

void AMD::FanCurveXMLParser::Initializer::takeFanCurveFanStop(bool enabled)
{
  outer_.fanStop_ = outer_.fanStopDefault_ = enabled;
}

void AMD::FanCurveXMLParser::Initializer::takeFanCurveFanStartValue(
    units::concentration::percent_t value)
{
  outer_.fanStartValue_ = outer_.fanStartValueDefault_ = value * 100;
}

AMD::FanCurveXMLParser::FanCurveXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::FanCurve::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::FanCurveXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::FanCurveXMLParser::initializer()
{
  return std::make_unique<AMD::FanCurveXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanCurveXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanCurveXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::FanCurveXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::FanCurveXMLParser::provideActive() const
{
  return active_;
}
void AMD::FanCurveXMLParser::takeFanCurvePoints(
    std::vector<AMD::FanCurve::Point> const &points)
{
  points_ = points;
}

std::vector<AMD::FanCurve::Point> const &
AMD::FanCurveXMLParser::provideFanCurvePoints() const
{
  return points_;
}

void AMD::FanCurveXMLParser::takeFanCurveFanStop(bool enabled)
{
  fanStop_ = enabled;
}

bool AMD::FanCurveXMLParser::provideFanCurveFanStop() const
{
  return fanStop_;
}

void AMD::FanCurveXMLParser::takeFanCurveFanStartValue(
    units::concentration::percent_t value)
{
  fanStartValue_ = value * 100;
}

units::concentration::percent_t
AMD::FanCurveXMLParser::provideFanCurveFanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanCurveXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(id_.c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("fanStop") = fanStop_;
  pmFixedNode.append_attribute("fanStartValue") = fanStartValue_;
  auto curveNode = pmFixedNode.append_child(CurveNodeName.data());
  for (auto &[temp, pwm] : points_) {
    auto pointNode = curveNode.append_child(PointNodeName.data());
    pointNode.append_attribute("temp") = temp.to<int>();
    pointNode.append_attribute("pwm") =
        static_cast<unsigned int>(std::round(pwm.to<double>() * 100));
  }
}

void AMD::FanCurveXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  points_ = pointsDefault_;
  fanStop_ = fanStopDefault_;
  fanStartValue_ = fanStartValueDefault_;
}

void AMD::FanCurveXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  fanStop_ = pmFixedNode.attribute("fanStop").as_bool(fanStopDefault_);
  fanStartValue_ =
      pmFixedNode.attribute("fanStartValue").as_uint(fanStartValueDefault_);

  auto curveNode = pmFixedNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == CurveNodeName; });

  if (!curveNode) {
    points_ = pointsDefault_;
  }
  else {
    points_.clear();
    for (auto pointNode : curveNode.children(PointNodeName.data())) {
      auto tempAttr = pointNode.attribute("temp");
      auto pwmAttr = pointNode.attribute("pwm");
      if (tempAttr && pwmAttr) {
        points_.emplace_back(units::temperature::celsius_t(tempAttr.as_int()),
                             units::concentration::percent_t(pwmAttr.as_uint()));
      }
      else { // malformed point data -> restore defaults
        points_ = pointsDefault_;
        break;
      }
    }

    if (points_.size() < 2) // two or more points are needed
      points_ = pointsDefault_;
  }
}

bool const AMD::FanCurveXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::FanCurve::ItemID, []() {
      return std::make_unique<AMD::FanCurveXMLParser>();
    });
