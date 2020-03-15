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

  std::string const id_;

  bool active_;
  bool activeDefault_;

  std::string color_;
  std::string colorDefault_;
};
