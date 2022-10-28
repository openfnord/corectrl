// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverdriveqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmoverdrive.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

AMD::PMOverdriveQMLItem::PMOverdriveQMLItem() noexcept
: ControlGroupQMLItem(AMD::PMOverdrive::ItemID)
{
}

bool AMD::PMOverdriveQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMOverdriveQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                             AMD::PMOverdrive::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMOverdrive::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMOverdriveForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMOverdriveQMLItem::registered_ =
    AMD::PMOverdriveQMLItem::register_();
