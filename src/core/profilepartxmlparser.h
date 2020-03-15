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

#include "exportable.h"
#include "importable.h"
#include "iprofilepartxmlparser.h"
#include "pugixml/pugixml.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Item;
class IProfilePartXMLParserProvider;

class ProfilePartXMLParser : public IProfilePartXMLParser
{
 public:
  ProfilePartXMLParser(Importable::Importer &profilePartImporter,
                       Exportable::Exporter &profilePartExporter) noexcept;

  void loadFrom(pugi::xml_node const &parentNode) final override;
  Importable::Importer &profilePartImporter() const final override;
  Exportable::Exporter &profilePartExporter() const final override;

  class Factory
  {
   public:
    Factory(IProfilePartXMLParserProvider const &profilePartParserProvider) noexcept;

    std::optional<std::reference_wrapper<Exportable::Exporter>>
    factory(Item const &i);

    virtual void takePartParser(Item const &i,
                                std::unique_ptr<IProfilePartXMLParser> &&part) = 0;
    virtual ~Factory() = default;

   private:
    std::unique_ptr<IProfilePartXMLParser>
    createPartParser(std::string const &componentID) const;

    IProfilePartXMLParserProvider const &profilePartParserProvider_;
    std::vector<std::unique_ptr<Exportable::Exporter>> factories_;
  };

 protected:
  virtual void resetAttributes() = 0;
  virtual void loadPartFrom(pugi::xml_node const &parentNode) = 0;

 private:
  Importable::Importer &importer_;
  Exportable::Exporter &exporter_;
};
