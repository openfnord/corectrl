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

#include "core/iprofilepart.h"
#include "core/isyscomponentprofilepart.h"
#include <string>

class IGPUProfilePart : public ISysComponentProfilePart
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &provideDeviceID() const = 0;
    virtual std::string const &provideRevision() const = 0;
    virtual int provideIndex() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeIndex(int index) = 0;
    virtual void takeDeviceID(std::string const &deviceID) = 0;
    virtual void takeRevision(std::string const &revision) = 0;
  };

  virtual ~IGPUProfilePart() = default;
};
