// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
