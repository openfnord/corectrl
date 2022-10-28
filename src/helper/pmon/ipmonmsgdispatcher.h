// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

class IPMonMsgDispatcher
{
 public:
  virtual void sendExec(std::string const &app) = 0;
  virtual void sendExit(std::string const &app) = 0;

  virtual ~IPMonMsgDispatcher() = default;
};
