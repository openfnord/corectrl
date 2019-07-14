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

#include "isysmodel.h"
#include <memory>
#include <string>
#include <vector>

class IProfilePart;
class IProfilePartView;

class IProfileView : public ISysModel::Importer
{
 public:
  class View
  {
   public:
    virtual std::string const &name() const = 0;
    virtual std::vector<std::shared_ptr<IProfilePart>> const &parts() const = 0;

    virtual ~View() = default;
  };

  virtual std::string const &name() const = 0;
  virtual std::vector<std::unique_ptr<IProfilePartView>> const &parts() const = 0;

  virtual ~IProfileView() = default;
};
