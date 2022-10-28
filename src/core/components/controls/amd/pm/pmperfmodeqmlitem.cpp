// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmperfmodeqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmperfmode.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

AMD::PMPerfModeQMLItem::PMPerfModeQMLItem() noexcept
: ControlModeQMLItem(AMD::PMPerfMode::ItemID)
{
}

bool AMD::PMPerfModeQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMPerfModeQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                            AMD::PMPerfMode::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMPerfMode::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMPerfModeForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMPerfModeQMLItem::registered_ =
    AMD::PMPerfModeQMLItem::register_();
