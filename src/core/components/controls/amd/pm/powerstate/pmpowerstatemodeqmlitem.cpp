// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerstatemodeqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmpowerstatemode.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

AMD::PMPowerStateModeQMLItem::PMPowerStateModeQMLItem() noexcept
: ControlModeQMLItem(AMD::PMPowerStateMode::ItemID)
{
}

bool AMD::PMPowerStateModeQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMPowerStateModeQMLItem>(
        "CoreCtrl.UIComponents", 1, 0, AMD::PMPowerStateMode::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMPowerStateMode::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMPowerStateModeForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMPowerStateModeQMLItem::registered_ =
    AMD::PMPowerStateModeQMLItem::register_();
