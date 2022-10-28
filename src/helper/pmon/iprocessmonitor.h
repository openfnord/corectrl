// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

struct ProcessEvent;

class IProcessMonitor
{
 public:
  virtual void start() = 0;
  virtual void stop() = 0;

  virtual ~IProcessMonitor() = default;

 protected:
  virtual ProcessEvent waitProcessEvent() = 0;
};
