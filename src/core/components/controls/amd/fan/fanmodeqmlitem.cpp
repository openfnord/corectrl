// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanmodeqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "fanmode.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

AMD::FanModeQMLItem::FanModeQMLItem() noexcept
: ControlModeQMLItem(AMD::FanMode::ItemID)
{
}

bool AMD::FanModeQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::FanModeQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                         AMD::FanMode::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::FanMode::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDFanModeForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::FanModeQMLItem::registered_ = AMD::FanModeQMLItem::register_();
