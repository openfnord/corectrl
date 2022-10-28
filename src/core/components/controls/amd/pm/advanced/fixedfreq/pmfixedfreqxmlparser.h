// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "pmfixedfreqprofilepart.h"
#include <string>

namespace AMD {

class PMFixedFreqXMLParser final
: public ProfilePartXMLParser
, public AMD::PMFixedFreqProfilePart::Exporter
, public AMD::PMFixedFreqProfilePart::Importer
{
 public:
  PMFixedFreqXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFixedFreqSclkIndex(unsigned int index) override;
  unsigned int providePMFixedFreqSclkIndex() const override;

  void takePMFixedFreqMclkIndex(unsigned int index) override;
  unsigned int providePMFixedFreqMclkIndex() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Initializer;

  bool active_;
  bool activeDefault_;

  unsigned int sclkIndex_;
  unsigned int sclkIndexDefault_;

  unsigned int mclkIndex_;
  unsigned int mclkIndexDefault_;

  static bool const registered_;
};

} // namespace AMD
