// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

class IAppRegistry;

class ProcessRegistry final
{
 public:
  ProcessRegistry(std::shared_ptr<IAppRegistry> appRegistry) noexcept;

  void add(int PID, std::string const &app);
  void remove(int PID);

  bool running(std::string const &app);
  std::optional<std::string> findApp(int PID);

 private:
  void registeredApp(std::string const &app);
  void unregisteredApp(std::string const &app);

  class AppRegistryObserver;
  std::shared_ptr<AppRegistryObserver> const appRegObserver_;
  std::unordered_map<int, std::string> pidReg_;
  std::unordered_map<std::string, int> appCounterReg_;
  std::mutex mutex_;
};
