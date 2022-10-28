// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "pmfreqodprofilepart.h"
#include <string>

namespace AMD {

class PMFreqOdXMLParser final
: public ProfilePartXMLParser
, public AMD::PMFreqOdProfilePart::Exporter
, public AMD::PMFreqOdProfilePart::Importer
{
 public:
  PMFreqOdXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFreqOdSclkOd(unsigned int value) override;
  unsigned int providePMFreqOdSclkOd() const override;

  void takePMFreqOdMclkOd(unsigned int value) override;
  unsigned int providePMFreqOdMclkOd() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Initializer;

  bool active_;
  bool activeDefault_;

  unsigned int sclkOd_;
  unsigned int sclkOdDefault_;

  unsigned int mclkOd_;
  unsigned int mclkOdDefault_;

  static bool const registered_;
};

} // namespace AMD
