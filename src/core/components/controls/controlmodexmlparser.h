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

#include "controlmodeprofilepart.h"
#include "core/profilepartxmlparser.h"
#include "pugixml/pugixml.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class ControlModeXMLParser
: public ProfilePartXMLParser
, public ControlModeProfilePart::Exporter
, public ControlModeProfilePart::Importer
{
 public:
  ControlModeXMLParser(std::string_view id) noexcept;

  std::unique_ptr<Exportable::Exporter> factory(
      IProfilePartXMLParserProvider const &profilePartParserProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) final override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) final override;

  void takeActive(bool active) final override;
  bool provideActive() const final override;

  void takeMode(std::string const &mode) final override;
  std::string const &provideMode() const final override;

  void appendTo(pugi::xml_node &parentNode) final override;

 protected:
  void resetAttributes() final override;
  void loadPartFrom(pugi::xml_node const &parentNode) override;

  void loadComponents(pugi::xml_node const &parentNode);
  bool activeDefault() const;
  std::string const &modeDefault() const;

 private:
  class Factory;
  class Initializer;

  std::unordered_map<std::string, std::unique_ptr<IProfilePartXMLParser>> parsers_;

  bool active_;
  bool activeDefault_;

  std::string mode_;
  std::string modeDefault_;
};
