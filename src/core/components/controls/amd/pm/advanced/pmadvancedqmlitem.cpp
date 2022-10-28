// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmadvancedqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmadvanced.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>

char const *const AMD::PMAdvancedQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_PM_ADVANCED"),
};

AMD::PMAdvancedQMLItem::PMAdvancedQMLItem() noexcept
: ControlGroupQMLItem(AMD::PMAdvanced::ItemID)
{
}

bool AMD::PMAdvancedQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMAdvancedQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                            AMD::PMAdvanced::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMAdvanced::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMAdvancedForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMAdvancedQMLItem::registered_ =
    AMD::PMAdvancedQMLItem::register_();
