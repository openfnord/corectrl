// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "cpufreqprofilepart.h"
#include <string>

class CPUFreqXMLParser final
: public ProfilePartXMLParser
, public CPUFreqProfilePart::Exporter
, public CPUFreqProfilePart::Importer
{
 public:
  CPUFreqXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takeCPUFreqScalingGovernor(std::string const &governor) override;
  std::string const &provideCPUFreqScalingGovernor() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Initializer;

  std::string const id_;

  bool active_;
  bool activeDefault_;

  std::string scalingGovernor_;
  std::string scalingGovernorDefault_;

  static bool const registered_;
};
