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
#pragma once

#include "core/profilepartxmlparser.h"
#include "pmvoltcurveprofilepart.h"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace AMD {

class PMVoltCurveXMLParser final
: public ProfilePartXMLParser
, public AMD::PMVoltCurveProfilePart::Exporter
, public AMD::PMVoltCurveProfilePart::Importer
{
 public:
  PMVoltCurveXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMVoltCurveMode(std::string const &mode) override;
  std::string const &providePMVoltCurveMode() const override;

  void takePMVoltCurvePoints(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &points) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMVoltCurvePoint(unsigned int index) const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;
  void savePoints(pugi::xml_node &node) const;
  void loadPoints(pugi::xml_node &node);
  void loadPointsFromLegacyNode(pugi::xml_node &node);

 private:
  static constexpr std::string_view LegacyPointsNodeName{"VOLT_CURVE"};
  static constexpr std::string_view LegacyModeAttribute{"voltMode"};
  static constexpr std::string_view PointNodeName{"POINT"};

  class Initializer;

  bool active_;
  bool activeDefault_;

  std::string mode_;
  std::string modeDefault_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      points_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      pointsDefault_;

  static bool const registered_;
};

} // namespace AMD
