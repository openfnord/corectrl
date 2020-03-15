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
#include "processregistry.h"

#include "iappregistry.h"
#include <memory>
#include <utility>

class ProcessRegistry::AppRegistryObserver : public IAppRegistry::Observer
{
 public:
  AppRegistryObserver(ProcessRegistry &outer) noexcept;
  void registeredApp(std::string const &app) override;
  void unregisteredApp(std::string const &app) override;

 private:
  ProcessRegistry &outer_;
};

ProcessRegistry::AppRegistryObserver::AppRegistryObserver(
    ProcessRegistry &outer) noexcept
: outer_(outer)
{
}

void ProcessRegistry::AppRegistryObserver::registeredApp(std::string const &app)
{
  outer_.registeredApp(app);
}

void ProcessRegistry::AppRegistryObserver::unregisteredApp(std::string const &app)
{
  outer_.unregisteredApp(app);
}

ProcessRegistry::ProcessRegistry(std::shared_ptr<IAppRegistry> appRegistry) noexcept
: appRegObserver_(std::make_shared<ProcessRegistry::AppRegistryObserver>(*this))
{
  appRegistry->addObserver(appRegObserver_);
}

void ProcessRegistry::add(int PID, std::string const &app)
{
  std::lock_guard<std::mutex> lock(mutex_);

  auto const pidIt = pidReg_.find(PID);
  if (pidIt == pidReg_.cend()) {
    pidReg_.insert({PID, app});

    // increment counter
    auto const appIt = appCounterReg_.find(app);
    if (appIt != appCounterReg_.cend())
      ++appIt->second;
  }
}

void ProcessRegistry::remove(int PID)
{

  std::lock_guard<std::mutex> lock(mutex_);

  auto const pidIt = pidReg_.find(PID);
  if (pidIt != pidReg_.cend()) {
    auto const app = std::move(pidIt->second);
    pidReg_.erase(pidIt);

    // decrement counter
    auto appIt = appCounterReg_.find(app);
    if (appIt != appCounterReg_.end())
      --appIt->second;
  }
}

bool ProcessRegistry::running(std::string const &app)
{
  std::lock_guard<std::mutex> lock(mutex_);

  auto const it = appCounterReg_.find(app);
  if (it != appCounterReg_.cend())
    return it->second > 0;

  return false;
}

std::optional<std::string> ProcessRegistry::findApp(int PID)
{
  std::lock_guard<std::mutex> lock(mutex_);

  auto const it = pidReg_.find(PID);
  if (it != pidReg_.end())
    return it->second;

  return {};
}

void ProcessRegistry::registeredApp(std::string const &app)
{
  std::lock_guard<std::mutex> lock(mutex_);
  appCounterReg_.insert({app, 0});
}

void ProcessRegistry::unregisteredApp(std::string const &app)
{
  std::lock_guard<std::mutex> lock(mutex_);

  for (auto it = pidReg_.begin(); it != pidReg_.end();) {
    if (it->second == app)
      it = pidReg_.erase(it);
    else
      ++it;
  }

  appCounterReg_.erase(app);
}
