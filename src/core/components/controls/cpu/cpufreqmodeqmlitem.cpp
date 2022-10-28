// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpufreqmodeqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "cpufreqmode.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>

CPUFreqModeQMLItem::CPUFreqModeQMLItem() noexcept
: ControlModeQMLItem(CPUFreqMode::ItemID)
{
}

bool CPUFreqModeQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<CPUFreqModeQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                        CPUFreqMode::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      CPUFreqMode::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/CPUFreqModeForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const CPUFreqModeQMLItem::registered_ = CPUFreqModeQMLItem::register_();
