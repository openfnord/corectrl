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
#include "processmonitor.h"

#include "iappregistry.h"
#include "ipidsolver.h"
#include "ipmonmsgdispatcher.h"
#include "processevent.h"
#include "processregistry.h"
#include <utility>

ProcessMonitor::ProcessMonitor(
    std::shared_ptr<IAppRegistry> appRegistry,
    std::unique_ptr<IPIDSolver> &&pidSolver,
    std::unique_ptr<IPMonMsgDispatcher> &&msgDispatcher) noexcept
: appRegistry_(std::move(appRegistry))
, pidSolver_(std::move(pidSolver))
, msgDispatcher_(std::move(msgDispatcher))
{
  procRegistry_ = std::make_unique<ProcessRegistry>(this->appRegistry_);
}

ProcessMonitor::~ProcessMonitor() = default;

void ProcessMonitor::start()
{
  while (!stopSignal_.load(std::memory_order_relaxed)) {
    ProcessEvent const e = waitProcessEvent();

    switch (e.event) {
      case ProcessEvent::Type::IGNORE:
        continue;

      case ProcessEvent::Type::EXEC:
        processExec(e.pid);
        break;

      case ProcessEvent::Type::EXIT:
        processExit(e.pid);
        break;
    }
  }
}

void ProcessMonitor::stop()
{
  stopSignal_.store(true, std::memory_order_relaxed);
}

void ProcessMonitor::processExec(int pid)
{
  auto const app = pidSolver_->app(pid);

  if (!appRegistry_->registered(app))
    return;

  if (!procRegistry_->running(app))
    msgDispatcher_->sendExec(app);

  procRegistry_->add(pid, app);
}

void ProcessMonitor::processExit(int pid)
{
  auto app = procRegistry_->findApp(pid);
  if (!app) // no app registered to that PID
    return;

  procRegistry_->remove(pid);

  if (!procRegistry_->running(app.value()))
    msgDispatcher_->sendExit(app.value());
}
