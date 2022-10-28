// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "nlprocexecsocket.h"
#include "processmonitor.h"
#include <memory>

class IAppRegistry;
class IPIDSolver;
class IPMonMsgDispatcher;

class NLProcExecMonitor final : public ProcessMonitor
{
 public:
  NLProcExecMonitor(std::shared_ptr<IAppRegistry> appRegistry,
                    std::unique_ptr<IPIDSolver> &&pidSolver,
                    std::unique_ptr<IPMonMsgDispatcher> &&msgDispatcher) noexcept;

 protected:
  ProcessEvent waitProcessEvent() override;

 private:
  NLProcExecSocket nlSock_;
};
