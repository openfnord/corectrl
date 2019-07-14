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

#include "iprocessmonitor.h"
#include <atomic>
#include <memory>

class IAppRegistry;
class IPIDSolver;
class IPMonMsgDispatcher;
class ProcessRegistry;

class ProcessMonitor : public IProcessMonitor
{
 public:
  ProcessMonitor(std::shared_ptr<IAppRegistry> appRegistry,
                 std::unique_ptr<IPIDSolver> &&pidSolver,
                 std::unique_ptr<IPMonMsgDispatcher> &&msgDispatcher) noexcept;

  ~ProcessMonitor() override;

  void start() override;
  void stop() override;

 private:
  void processExec(int pid);
  void processExit(int pid);

  std::shared_ptr<IAppRegistry> const appRegistry_;
  std::unique_ptr<IPIDSolver> const pidSolver_;
  std::unique_ptr<ProcessRegistry> procRegistry_;
  std::unique_ptr<IPMonMsgDispatcher> msgDispatcher_;

  std::atomic<bool> stopSignal_{false};
};
