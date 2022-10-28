// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepartxmlparser.h"
#include "igpuprofilepart.h"
#include "pugixml/pugixml.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class IProfilePartXMLParserProvider;

class GPUXMLParser final
: public ProfilePartXMLParser
, public IGPUProfilePart::Exporter
, public IGPUProfilePart::Importer
{
 public:
  GPUXMLParser() noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takeIndex(int index) override;
  int provideIndex() const override;

  void takeDeviceID(std::string const &deviceID) override;
  std::string const &provideDeviceID() const override;

  void takeRevision(std::string const &revision) override;
  std::string const &provideRevision() const override;

  void takeUniqueID(std::optional<std::string> uniqueID) override;
  std::optional<std::string> provideUniqueID() const override;

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

  std::string deviceID_;
  std::string deviceIDDefault_;
  std::string revision_;
  std::string revisionDefault_;
  std::optional<std::string> uniqueID_;
  std::optional<std::string> uniqueIDDefault_;
  int index_;
  int indexDefault_;

  static bool const registered_;
};
