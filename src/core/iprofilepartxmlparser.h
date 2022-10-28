// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "exportable.h"
#include "importable.h"
#include "item.h"
#include "pugixml/pugixml.hpp"
#include <memory>

class IProfilePartXMLParserProvider;

class IProfilePartXMLParser : public Item
{
 public:
  virtual std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartXMLParserProvider const &profilePartXMLParserProvider) = 0;
  virtual std::unique_ptr<Exportable::Exporter> initializer() = 0;

  virtual void loadFrom(pugi::xml_node const &parentNode) = 0;
  virtual void appendTo(pugi::xml_node &parentNode) = 0;

  virtual Importable::Importer &profilePartImporter() const = 0;
  virtual Exportable::Exporter &profilePartExporter() const = 0;

  virtual ~IProfilePartXMLParser() = default;
};
