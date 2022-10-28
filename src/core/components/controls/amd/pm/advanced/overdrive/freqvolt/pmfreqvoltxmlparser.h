// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "pmfreqvoltprofilepart.h"
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace AMD {

class PMFreqVoltXMLParser final
: public ProfilePartXMLParser
, public AMD::PMFreqVoltProfilePart::Exporter
, public AMD::PMFreqVoltProfilePart::Importer
{
 public:
  PMFreqVoltXMLParser() noexcept;

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

  void takePMFreqVoltControlName(std::string const &name) override;
  void takePMFreqVoltVoltMode(std::string const &mode) override;
  void takePMFreqVoltStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFreqVoltActiveStates(std::vector<unsigned int> const &states) override;

  std::string const &providePMFreqVoltVoltMode() const override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFreqVoltState(unsigned int index) const override;
  std::vector<unsigned int> providePMFreqVoltActiveStates() const override;

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

  std::string voltMode_;
  std::string voltModeDefault_;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      states_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      statesDefault_;

  std::vector<unsigned int> activeStates_;
  std::vector<unsigned int> activeStatesDefault_;

  static bool const registered_;
};

} // namespace AMD
