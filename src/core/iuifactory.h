// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

class ISession;
class ISysModel;
class QQmlApplicationEngine;

class IUIFactory
{
 public:
  virtual void build(QQmlApplicationEngine &qmlEngine,
                     ISysModel const &sysModel, ISession &session) const = 0;

  virtual ~IUIFactory() = default;
};
