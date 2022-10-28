// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "icpuprofilepart.h"
#include "pugixml/pugixml.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class CPUXMLParser final
: public ProfilePartXMLParser
, public ICPUProfilePart::Exporter
, public ICPUProfilePart::Importer
{
 public:
  CPUXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takeSocketId(int id) override;
  int provideSocketId() const override;

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Factory;
  class Initializer;

  std::unordered_map<std::string, std::unique_ptr<IProfilePartXMLParser>> parsers_;

  bool active_;
  bool activeDefault_;

  int socketId_;
  int socketIdDefault_;

  static bool const registered_;
};
