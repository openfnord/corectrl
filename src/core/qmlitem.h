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

#include "exportable.h"
#include <QObject>
#include <QQuickItem>
#include <QString>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class IQMLComponentFactory;
class QQmlApplicationEngine;

class QMLItem : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name NOTIFY nameChanged)

 public:
  /// Suffix used for QMLItem parent objects in QML files
  static constexpr std::string_view ParentObjectSuffix{"_Plug"};

  /// @returns item's name
  QString const &name() const;

  /// @returns Initializer of the derived class
  virtual std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) = 0;

  virtual ~QMLItem() = default;

  Q_INVOKABLE void setupChild(QQuickItem *child);
  Q_INVOKABLE virtual void activate(bool active) = 0;

 signals:
  void nameChanged();
  void settingsChanged();

 protected:
  /// Sets the item's name
  void setName(QString const &name);

 public:
  /// QMLItem specializations should extend this class in their
  /// Initializer specializations
  class Initializer
  {
   public:
    Initializer(IQMLComponentFactory const &qmlComponentFactory,
                QQmlApplicationEngine &qmlEngine) noexcept;

    /// @returns A pair with an instance of the initializer for the QMLItem
    /// registered with the itemID and a pointer to the created QMLItem
    std::pair<std::optional<std::reference_wrapper<Exportable::Exporter>>, QMLItem *>
    initializer(std::string const &itemID, QMLItem *parent);

    virtual ~Initializer() = default;

   protected:
    IQMLComponentFactory const &qmlComponentFactory_;

   private:
    QQmlApplicationEngine &qmlEngine_;
    std::vector<std::unique_ptr<Exportable::Exporter>> initializers_;
  };

 private:
  QString name_;
};
