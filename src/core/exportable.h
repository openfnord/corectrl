// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <functional>
#include <optional>

class Item;

class Exportable
{
 public:
  class Exporter
  {
   public:
    virtual std::optional<std::reference_wrapper<Exportable::Exporter>>
    provideExporter(Item const &i) = 0;

    virtual ~Exporter() = default;
  };

  virtual void exportWith(Exportable::Exporter &e) const = 0;

  virtual ~Exportable() = default;
};
