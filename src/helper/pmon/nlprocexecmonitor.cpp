// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "nlprocexecmonitor.h"

#include "processevent.h"
#include <utility>

NLProcExecMonitor::NLProcExecMonitor(
    std::shared_ptr<IAppRegistry> appRegistry,
    std::unique_ptr<IPIDSolver> &&pidSolver,
    std::unique_ptr<IPMonMsgDispatcher> &&msgDispatcher) noexcept
: ProcessMonitor(std::move(appRegistry), std::move(pidSolver),
                 std::move(msgDispatcher))
{
}

ProcessEvent NLProcExecMonitor::waitProcessEvent()
{
  return nlSock_.waitForEvent();
}
