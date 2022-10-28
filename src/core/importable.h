// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <functional>
#include <optional>

class Item;

class Importable
{
 public:
  class Importer
  {
   public:
    virtual std::optional<std::reference_wrapper<Importable::Importer>>
    provideImporter(Item const &i) = 0;

    virtual ~Importer() = default;
  };

  virtual void importWith(Importable::Importer &i) = 0;

  virtual ~Importable() = default;
};
