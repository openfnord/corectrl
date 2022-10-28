// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqmodeqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfreqmode.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

AMD::PMFreqModeQMLItem::PMFreqModeQMLItem() noexcept
: ControlModeQMLItem(AMD::PMFreqMode::ItemID)
{
}

bool AMD::PMFreqModeQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFreqModeQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                            AMD::PMFreqMode::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFreqMode::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMFreqModeForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFreqModeQMLItem::registered_ =
    AMD::PMFreqModeQMLItem::register_();
