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
    profileManagerUI->init(&session.profileManager(),
                           dynamic_cast<ISysModelUI *>(item));

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
