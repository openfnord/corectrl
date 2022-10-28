// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

class QMLItem;
class QQmlApplicationEngine;
class QQuickItem;

class IQMLComponentFactory
{
 public:
  virtual void registerQMLTypes() const = 0;

  virtual QMLItem *createQMLItem(std::string const &itemID, QQuickItem *parent,
                                 QQmlApplicationEngine &qmlEngine) const = 0;

  virtual QQuickItem *
  createQuickItem(std::string const &itemID, QQuickItem *parent,
                  std::string const &parentObjectName) const = 0;

  virtual ~IQMLComponentFactory() = default;
};
