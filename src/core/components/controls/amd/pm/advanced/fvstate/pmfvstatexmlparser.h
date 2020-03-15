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
#include "pmfvstateprofilepart.h"
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace AMD {

class PMFVStateXMLParser final
: public ProfilePartXMLParser
, public AMD::PMFVStateProfilePart::Exporter
, public AMD::PMFVStateProfilePart::Importer
{
 public:
  PMFVStateXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFVStateGPUVoltMode(std::string const &mode) override;
  std::string const &providePMFVStateGPUVoltMode() const override;

  void takePMFVStateMemVoltMode(std::string const &mode) override;
  std::string const &providePMFVStateMemVoltMode() const override;

  void takePMFVStateGPUStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateGPUState(unsigned int index) const override;

  void takePMFVStateMemStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateMemState(unsigned int index) const override;

  void
  takePMFVStateGPUActiveStates(std::vector<unsigned int> const &indices) override;
  std::vector<unsigned int> providePMFVStateMemActiveStates() const override;
  void
  takePMFVStateMemActiveStates(std::vector<unsigned int> const &indices) override;
  std::vector<unsigned int> providePMFVStateGPUActiveStates() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;
  void
  saveStates(pugi::xml_node &fvStateNode, std::string_view stateNodeId,
             std::string const &voltMode,
             std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                    units::voltage::millivolt_t>> &states,
             std::vector<unsigned int> &activeStates) const;
  void loadStates(
      pugi::xml_node const &fvStateNode, std::string_view stateNodeId,
      std::string &voltMode, std::string const &voltModeDefault,
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> &states,
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &statesDefault,
      std::vector<unsigned int> &activeStates,
      std::vector<unsigned int> const &activeStatesDefault) const;

 private:
  static constexpr std::string_view StatesNodeName{"STATES"};
  static constexpr std::string_view StateNodeName{"STATE"};
  static constexpr std::string_view GPUStateNodeId{"sclk"};
  static constexpr std::string_view MemStateNodeId{"mclk"};

  class Initializer;

  std::string const id_;

  bool active_;
  bool activeDefault_;

  std::string gpuVoltMode_;
  std::string gpuVoltModeDefault_;

  std::string memVoltMode_;
  std::string memVoltModeDefault_;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      gpuStates_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      gpuStatesDefault_;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      memStates_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      memStatesDefault_;

  std::vector<unsigned int> gpuActiveStates_;
  std::vector<unsigned int> gpuActiveStatesDefault_;

  std::vector<unsigned int> memActiveStates_;
  std::vector<unsigned int> memActiveStatesDefault_;

  static bool const registered_;
};

} // namespace AMD
