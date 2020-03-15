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

#include "core/exportable.h"
#include "core/importable.h"
#include "core/item.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class ICommandQueue;

class ISysModel
: public Item
, public Importable
, public Exportable
{
 public:
  static constexpr std::string_view ItemID{"SYS_MODEL"};

  class Importer : public Importable::Importer
  {
   public:
  };

  class Exporter : public Exportable::Exporter
  {
   public:
  };

  virtual void init() = 0;
  virtual void preInit(ICommandQueue &ctlCmds) = 0;
  virtual void postInit(ICommandQueue &ctlCmds) = 0;
  virtual void sync(ICommandQueue &ctlCmds) = 0;
  virtual void updateSensors(
      std::unordered_map<std::string, std::unordered_set<std::string>> const
          &ignored) = 0;

  virtual std::vector<
      std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
  info() const = 0;

  virtual ~ISysModel() = default;
};
