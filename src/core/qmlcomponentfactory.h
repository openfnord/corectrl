//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
