// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
