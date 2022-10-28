// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
