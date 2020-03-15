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
#include "appregistry.h"

#include <algorithm>
#include <utility>

void AppRegistry::add(std::string const &app)
{
  std::lock_guard<std::mutex> lock(regMutex_);
  registry_.insert(app);
  notifyRegisteredApp(app);
}

void AppRegistry::remove(std::string const &app)
{
  std::lock_guard<std::mutex> lock(regMutex_);
  if (registry_.erase(app) > 0)
    notifyUnregisteredApp(app);
}

bool AppRegistry::registered(std::string const &app)
{
  std::lock_guard<std::mutex> lock(regMutex_);
  return registry_.find(app) != registry_.end();
}

void AppRegistry::addObserver(std::shared_ptr<IAppRegistry::Observer> observer)
{
  std::lock_guard<std::mutex> lock(obMutex_);

  auto const it = std::find(observers_.begin(), observers_.end(), observer);
  if (it == observers_.end())
    observers_.emplace_back(std::move(observer));
}

void AppRegistry::removeObserver(
    std::shared_ptr<IAppRegistry::Observer> const &observer)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                   observers_.end());
}

void AppRegistry::notifyRegisteredApp(std::string const &app)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->registeredApp(app);
}

void AppRegistry::notifyUnregisteredApp(std::string const &app)
{
  std::lock_guard<std::mutex> lock(obMutex_);
  for (auto &o : observers_)
    o->unregisteredApp(app);
}
