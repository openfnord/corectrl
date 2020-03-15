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
#include <memory>
#include <string>
#include <string_view>

class IProfile
: public Item
, public Importable
, public Exportable
{
 public:
  static constexpr std::string_view ItemID{"PROFILE"};

  struct Info
  {
    static constexpr std::string_view GlobalID{"_global_"};
    static constexpr std::string_view GlobalIconURL{":/images/GlobalIcon"};
    static constexpr std::string_view DefaultIconURL{":/images/DefaultIcon"};

    Info(std::string name = "", std::string exe = "",
         std::string iconURL = std::string(DefaultIconURL))
    : name(name)
    , exe(exe)
    , iconURL(iconURL)
    {
    }

    std::string name{""};
    std::string exe{""};
    std::string iconURL{DefaultIconURL};
  };

  class Importer : public Importable::Importer
  {
   public:
    virtual bool provideActive() const = 0;
    virtual IProfile::Info const &provideInfo() const = 0;

    virtual ~Importer() = default;
  };

  class Exporter : public Exportable::Exporter
  {
   public:
    virtual void takeActive(bool active) = 0;
    virtual void takeInfo(Info const &info) = 0;

    virtual ~Exporter() = default;
  };

  virtual std::unique_ptr<Exportable::Exporter> initializer() = 0;

  virtual bool active() const = 0;
  virtual void activate(bool active) = 0;

  virtual IProfile::Info const &info() const = 0;
  virtual void info(IProfile::Info const &info) = 0;

  virtual std::unique_ptr<IProfile> clone() const = 0;

  virtual ~IProfile() = default;
};
