// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "pmpowerprofileprofilepart.h"
#include <string>

namespace AMD {

class PMPowerProfileXMLParser final
: public ProfilePartXMLParser
, public AMD::PMPowerProfileProfilePart::Exporter
, public AMD::PMPowerProfileProfilePart::Importer
{
 public:
  PMPowerProfileXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMPowerProfileMode(std::string const &mode) override;
  std::string const &providePMPowerProfileMode() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Initializer;

  bool active_;
  bool activeDefault_;

  std::string mode_;
  std::string modeDefault_;

  static bool const registered_;
};

} // namespace AMD
