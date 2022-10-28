// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iqmlcomponentfactory.h"
#include <memory>
#include <string_view>

class IQMLComponentRegistry;
class QQuickItem;

class QMLComponentFactory final : public IQMLComponentFactory
{
 public:
  QMLComponentFactory(
      std::unique_ptr<IQMLComponentRegistry> &&qmlComponentRegistry) noexcept;

  void registerQMLTypes() const override;

  QMLItem *createQMLItem(std::string const &itemID, QQuickItem *parent,
                         QQmlApplicationEngine &qmlEngine) const override;

  QQuickItem *createQuickItem(std::string const &itemID, QQuickItem *parent,
                              std::string const &parentObjectName) const override;

 private:
  /// Parent item to the first object that has parentObjectName as
  /// object name (the parent itself or one of its children)
  /// If there is no object with parentObjectName as object name,
  /// the item is parented to parent.
  void parentItem(QQuickItem *item, QQuickItem *parent,
                  std::string_view parentObjectName) const;

  std::unique_ptr<IQMLComponentRegistry> qmlComponentRegistry_;
};
