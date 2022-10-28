// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverclockqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmoverclock.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

AMD::PMOverclockQMLItem::PMOverclockQMLItem() noexcept
: ControlGroupQMLItem(AMD::PMOverclock::ItemID)
{
}

bool AMD::PMOverclockQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMOverclockQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                             AMD::PMOverclock::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMOverclock::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMOverclockForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMOverclockQMLItem::registered_ =
    AMD::PMOverclockQMLItem::register_();
