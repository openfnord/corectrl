// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <string>

class IHelperMonitor
{
 public:
  class Observer
  {
   public:
    virtual void appExec(std::string appExe) = 0;
    virtual void appExit(std::string appExe) = 0;

    virtual ~Observer() = default;
  };

  virtual void addObserver(std::shared_ptr<IHelperMonitor::Observer> observer) = 0;
  virtual void
  removeObserver(std::shared_ptr<IHelperMonitor::Observer> const &observer) = 0;

  virtual void init() = 0;

  virtual void watchApp(std::string const &app) = 0;
  virtual void forgetApp(std::string const &app) = 0;

  virtual ~IHelperMonitor() = default;
};
