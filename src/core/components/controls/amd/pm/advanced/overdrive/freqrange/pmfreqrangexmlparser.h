// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

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
