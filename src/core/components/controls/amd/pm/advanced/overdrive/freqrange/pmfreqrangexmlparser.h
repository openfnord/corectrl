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
#include "pmfreqrangeprofilepart.h"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace AMD {

class PMFreqRangeXMLParser final
: public ProfilePartXMLParser
, public AMD::PMFreqRangeProfilePart::Exporter
, public AMD::PMFreqRangeProfilePart::Importer
{
 public:
  PMFreqRangeXMLParser() noexcept;

  std::string const &instanceID() const final override;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFreqRangeControlName(std::string const &name) override;
  void takePMFreqRangeStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  units::frequency::megahertz_t
  providePMFreqRangeState(unsigned int index) const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;
  void saveStates(pugi::xml_node &node) const;
  void loadStates(pugi::xml_node const &node);
  void loadStatesFromLegacyNode(pugi::xml_node const &node);

 private:
  static constexpr std::string_view LegacyStatesNodeName{"STATES"};
  static constexpr std::string_view StateNodeName{"STATE"};

  class Initializer;

  bool active_;
  bool activeDefault_;

  std::string controlName_;
  std::string nodeID_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> statesDefault_;

  static bool const registered_;
};

} // namespace AMD
