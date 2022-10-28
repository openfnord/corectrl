// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "uifactory.h"

#include "iqmlcomponentfactory.h"
#include "isession.h"
#include "isysmodel.h"
#include "isysmodelui.h"
#include "profilemanagerui.h"
#include "qmlitem.h"
#include "sysmodelqmlitem.h"
#include "systeminfoui.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QString>
#include <QUrl>
#include <QtGlobal>
#include <stdexcept>
#include <string>
#include <utility>

UIFactory::UIFactory(
    std::unique_ptr<IQMLComponentFactory> &&qmlComponentfactory) noexcept
: qmlComponentFactory_(std::move(qmlComponentfactory))
{
}

void UIFactory::build(QQmlApplicationEngine &qmlEngine,
                      ISysModel const &sysModel, ISession &session) const
{
  qmlComponentFactory_->registerQMLTypes();

  // Use native font rendering
  // XXX On Qt 5.10, use QQuickWindow::setTextRenderType with native option
  // http://doc-snapshots.qt.io/qt5-5.10/qquickwindow.html#setTextRenderType
  qputenv("QML_DISABLE_DISTANCEFIELD", "1");

  qmlEngine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
  if (qmlEngine.rootObjects().isEmpty())
    throw std::runtime_error("QML engine failed to load main interface!");

  auto item = createSysModelQMLItem(qmlEngine);
  if (item != nullptr) {
    auto initializer = item->initializer(*qmlComponentFactory_, qmlEngine);
    sysModel.exportWith(*initializer);

    auto profileManagerUI = static_cast<ProfileManagerUI *>(
        qmlEngine.rootObjects().front()->findChild<QObject *>(
            ProfileManagerUI::QMLComponentID.data()));
    profileManagerUI->init(&session, dynamic_cast<ISysModelUI *>(item));

    auto systemInfoUI = static_cast<SystemInfoUI *>(
        qmlEngine.rootObjects().front()->findChild<QObject *>(
            SystemInfoUI::QMLComponentID.data()));
    systemInfoUI->init(&sysModel);
  }
}

QMLItem *UIFactory::createSysModelQMLItem(QQmlApplicationEngine &qmlEngine) const
{
  QString parentObjectName(SysModelQMLItem::ParentObjectName.data());
  parentObjectName.append(QMLItem::ParentObjectSuffix.data());

  auto itemParent = qmlEngine.rootObjects().front()->findChild<QQuickItem *>(
      parentObjectName);

  if (itemParent != nullptr)
    return qmlComponentFactory_->createQMLItem(std::string(ISysModel::ItemID),
                                               itemParent, qmlEngine);

  return nullptr;
}
