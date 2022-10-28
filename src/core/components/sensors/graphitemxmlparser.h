// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "graphitemprofilepart.h"
#include "pugixml/pugixml.hpp"
#include <string>
#include <string_view>

class GraphItemXMLParser final
: public ProfilePartXMLParser
, public GraphItemProfilePart::Exporter
, public GraphItemProfilePart::Importer
{
 public:
  GraphItemXMLParser(std::string_view id) noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(const Item &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(const Item &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takeColor(const std::string &color) override;
  std::string const &provideColor() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Initializer;

  bool active_;
  bool activeDefault_;

  std::string colorDefault_;
};
