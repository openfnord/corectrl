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
#include "pmfvvoltcurveprofilepart.h"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace AMD {

class PMFVVoltCurveXMLParser final
: public ProfilePartXMLParser
, public AMD::PMFVVoltCurveProfilePart::Exporter
, public AMD::PMFVVoltCurveProfilePart::Importer
{
 public:
  PMFVVoltCurveXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFVVoltCurveVoltMode(std::string const &mode) override;
  std::string const &providePMFVVoltCurveVoltMode() const override;

  void takePMFVVoltCurveVoltCurve(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &curve) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVVoltCurveVoltCurvePoint(unsigned int index) const override;

  void takePMFVVoltCurveGPUStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  units::frequency::megahertz_t
  providePMFVVoltCurveGPUState(unsigned int index) const override;

  void takePMFVVoltCurveMemStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  units::frequency::megahertz_t
  providePMFVVoltCurveMemState(unsigned int index) const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;
  void saveVoltCurve(pugi::xml_node &fvCurveVoltNode) const;
  void loadVoltCurve(pugi::xml_node &fvCurveVoltNode);
  void
  saveStates(pugi::xml_node &fvCurveVoltNode, std::string_view stateNodeId,
             std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
                 &states) const;
  void loadStates(
      pugi::xml_node const &fvCurveVoltNode, std::string_view stateNodeId,
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> &states,
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &statesDefault) const;

 private:
  static constexpr std::string_view VoltCurveNodeName{"VOLT_CURVE"};
  static constexpr std::string_view VoltCurvePointNodeName{"POINT"};
  static constexpr std::string_view StatesNodeName{"STATES"};
  static constexpr std::string_view StateNodeName{"STATE"};
  static constexpr std::string_view GPUStateNodeId{"sclk"};
  static constexpr std::string_view MemStateNodeId{"mclk"};

  class Initializer;

  std::string const id_;

  bool active_;
  bool activeDefault_;

  std::string voltMode_;
  std::string voltModeDefault_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurve_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurveDefault_;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuStates_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuStatesDefault_;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memStates_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memStatesDefault_;

  static bool const registered_;
};

} // namespace AMD
