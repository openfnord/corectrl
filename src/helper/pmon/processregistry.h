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
