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

#include <memory>
#include <string>

class IAppRegistry
{
 public:
  class Observer
  {
   public:
    virtual void registeredApp(std::string const &app) = 0;
    virtual void unregisteredApp(std::string const &app) = 0;

    virtual ~Observer() = default;
  };

  virtual void addObserver(std::shared_ptr<IAppRegistry::Observer> observer) = 0;
  virtual void
  removeObserver(std::shared_ptr<IAppRegistry::Observer> const &observer) = 0;

  virtual void add(std::string const &app) = 0;
  virtual void remove(std::string const &app) = 0;
  virtual bool registered(std::string const &app) = 0;

  virtual ~IAppRegistry() = default;
};
