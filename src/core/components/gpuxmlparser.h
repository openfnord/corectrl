//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "igpuprofilepart.h"
#include "pugixml/pugixml.hpp"
#include <memory>
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

  void appendTo(pugi::xml_node &parentNode) override;

 protected:
  void resetAttributes() override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

 private:
  class Factory;
  class Initializer;

  std::unordered_map<std::string, std::unique_ptr<IProfilePartXMLParser>> parsers_;

  std::string const id_;

  bool active_;
  bool activeDefault_;

  std::string deviceID_;
  std::string deviceIDDefault_;
  std::string revision_;
  std::string revisionDefault_;
  int index_;
  int indexDefault_;

  static bool const registered_;
};
