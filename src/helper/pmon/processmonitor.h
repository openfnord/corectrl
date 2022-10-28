// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
