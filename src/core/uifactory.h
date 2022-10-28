// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iuifactory.h"
#include <memory>

class QMLItem;
class IQMLComponentFactory;
class QQmlApplicationEngine;

class UIFactory final : public IUIFactory
{
 public:
  UIFactory(std::unique_ptr<IQMLComponentFactory> &&qmlComponentfactory) noexcept;

  void build(QQmlApplicationEngine &qmlEngine, ISysModel const &sysModel,
             ISession &session) const override;

 private:
  QMLItem *createSysModelQMLItem(QQmlApplicationEngine &qmlEngine) const;

  std::unique_ptr<IQMLComponentFactory> qmlComponentFactory_;
};
