// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "exportable.h"
#include "importable.h"
#include "iprofilepartxmlparser.h"
#include "pugixml/pugixml.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class Item;
class IProfilePartXMLParserProvider;

class ProfilePartXMLParser : public IProfilePartXMLParser
{
 public:
  ProfilePartXMLParser(std::string_view id,
                       Importable::Importer &profilePartImporter,
                       Exportable::Exporter &profilePartExporter) noexcept;

  std::string const &ID() const final override;

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
  std::string const id_;
  Importable::Importer &importer_;
  Exportable::Exporter &exporter_;
};
