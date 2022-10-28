// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <string>

class IAppRegistry
{
 public:
  class Observer
  {
   public:
    virtual void registeredApp(std::string const &app) = 0;
    virtual void unregisteredApp(std::string const &app) = 0;

    virtual ~Observer() = default;
  };

  virtual void addObserver(std::shared_ptr<IAppRegistry::Observer> observer) = 0;
  virtual void
  removeObserver(std::shared_ptr<IAppRegistry::Observer> const &observer) = 0;

  virtual void add(std::string const &app) = 0;
  virtual void remove(std::string const &app) = 0;
  virtual bool registered(std::string const &app) = 0;

  virtual ~IAppRegistry() = default;
};
