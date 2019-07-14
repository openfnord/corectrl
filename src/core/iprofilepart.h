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
#include "item.h"
#include <memory>

class IProfilePartProvider;

class IProfilePart
: public Item
, public Importable
, public Exportable
{
 public:
  class Importer : public Importable::Importer
  {
   public:
    virtual bool provideActive() const = 0;
  };

  class Exporter : public Exportable::Exporter
  {
   public:
    virtual void takeActive(bool active) = 0;
  };

  virtual std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) = 0;

  virtual std::unique_ptr<Exportable::Exporter> initializer() = 0;

  virtual bool active() const = 0;
  virtual void activate(bool active) = 0;

  virtual std::unique_ptr<IProfilePart> clone() const = 0;

  virtual ~IProfilePart() = default;
};
