// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
