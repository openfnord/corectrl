// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

class ICommandQueue;

class IHelperSysCtl
{
 public:
  virtual void init() = 0;
  virtual void apply(ICommandQueue &ctlCmds) = 0;

  virtual ~IHelperSysCtl() = default;
};
