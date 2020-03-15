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
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class ICommandQueue;

class ISysComponent
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

  virtual bool active() const = 0;
  virtual void activate(bool active) = 0;

  /// @return unique key generated for this system component
  virtual std::string const &key() const = 0;

  /// @return component information as a pair of the component name and
  /// a vector of pairs of key and info values.
  virtual std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
  componentInfo() const = 0;

  virtual void init() = 0;
  virtual void preInit(ICommandQueue &ctlCmds) = 0;
  virtual void postInit(ICommandQueue &ctlCmds) = 0;
  virtual void sync(ICommandQueue &ctlCmds) = 0;
  virtual void updateSensors(
      std::unordered_map<std::string, std::unordered_set<std::string>> const
          &ignored) = 0;

  virtual ~ISysComponent() = default;
};
