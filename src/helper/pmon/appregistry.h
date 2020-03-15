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

#include "iappregistry.h"

#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

class AppRegistry final : public IAppRegistry
{
 public:
  void addObserver(std::shared_ptr<IAppRegistry::Observer> observer) override;
  void removeObserver(
      std::shared_ptr<IAppRegistry::Observer> const &observer) override;

  void add(std::string const &app) override;
  void remove(std::string const &app) override;
  bool registered(std::string const &app) override;

 private:
  void notifyRegisteredApp(std::string const &app);
  void notifyUnregisteredApp(std::string const &app);

  std::vector<std::shared_ptr<IAppRegistry::Observer>> observers_;
  std::mutex obMutex_;

  std::unordered_set<std::string> registry_;
  std::mutex regMutex_;
};
