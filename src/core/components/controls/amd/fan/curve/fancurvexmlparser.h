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
#pragma once

#include "core/profilepartxmlparser.h"
#include "fancurveprofilepart.h"
#include <string>
#include <string_view>
#include <vector>

namespace AMD {

class FanCurveXMLParser final
: public ProfilePartXMLParser
, public AMD::FanCurveProfilePart::Exporter
, public AMD::FanCurveProfilePart::Importer
{
 public:
  FanCurveXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takeFanCurvePoints(std::vector<FanCurve::Point> const &points) override;
  std::vector<FanCurve::Point> const &provideFanCurvePoints() const override;

  void takeFanCurveFanStop(bool enabled) override;
  bool provideFanCurveFanStop() const override;

  void takeFanCurveFanStartValue(units::concentration::percent_t value) override;
  units::concentration::percent_t provideFanCurveFanStartValue() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  static constexpr std::string_view CurveNodeName{"CURVE"};
  static constexpr std::string_view PointNodeName{"POINT"};

  class Initializer;

  std::string const id_;

  bool active_;
  bool activeDefault_;

  std::vector<FanCurve::Point> points_;
  std::vector<FanCurve::Point> pointsDefault_;

  bool fanStop_;
  bool fanStopDefault_;

  unsigned int fanStartValue_;
  unsigned int fanStartValueDefault_;

  static bool const registered_;
};

} // namespace AMD
