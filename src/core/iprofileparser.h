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
#include <memory>
#include <string>
#include <vector>

class IProfile;

class IProfileParser
{
 public:
  virtual std::string const &format() = 0;

  virtual std::unique_ptr<Exportable::Exporter> initializer() = 0;

  virtual bool load(std::vector<char> const &data, IProfile &profile) = 0;
  virtual bool save(std::vector<char> &data, IProfile const &profile) = 0;

  virtual ~IProfileParser() = default;
};
