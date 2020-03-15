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
#include "qmlcomponentfactory.h"

#include "qmlcomponentregistry.h"
#include "qmlitem.h"
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQuickItem>
#include <QString>
#include <utility>

QMLComponentFactory::QMLComponentFactory(
    std::unique_ptr<IQMLComponentRegistry> &&qmlComponentRegistry) noexcept
: qmlComponentRegistry_(std::move(qmlComponentRegistry))
{
}

void QMLComponentFactory::registerQMLTypes() const
{
  auto &qmlTypeRegisterers = qmlComponentRegistry_->qmlTypeRegisterers();
  for (auto &qmlTypeRegisterer : qmlTypeRegisterers)
    qmlTypeRegisterer();
}

QMLItem *QMLComponentFactory::createQMLItem(std::string const &itemID,
                                            QQuickItem *parent,
                                            QQmlApplicationEngine &qmlEngine) const
{
  auto &qmlItemProviders = qmlComponentRegistry_->qmlItemProviders();
  auto const providerIt = qmlItemProviders.find(itemID);
  if (providerIt != qmlItemProviders.cend()) {
    auto item = providerIt->second(qmlEngine);
    QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);

    auto parentName = parent->objectName();
    if (!parentName.contains(QMLItem::ParentObjectSuffix.data()))
      parentName.append(QMLItem::ParentObjectSuffix.data());

    parentItem(item, parent, parentName.toStdString());

    return item;
  }

  return nullptr;
}

QQuickItem *
QMLComponentFactory::createQuickItem(std::string const &itemID,
                                     QQuickItem *parent,
                                     std::string const &parentObjectName) const
{
  auto quickItemProviders = qmlComponentRegistry_->quickItemProviders();
  auto providerIt = quickItemProviders.find(itemID);
  if (providerIt != quickItemProviders.cend()) {
    auto item = providerIt->second();
    QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
    parentItem(item, parent, parentObjectName);

    return item;
  }

  return nullptr;
}

void QMLComponentFactory::parentItem(QQuickItem *item, QQuickItem *parent,
                                     std::string_view parentObjectName) const
{
  QQuickItem *parentItem = parent;
  if (parent->objectName() != parentObjectName.data()) {
    parentItem = parent->findChild<QQuickItem *>(parentObjectName.data());
    if (parentItem == nullptr)
      parentItem = parent;
  }
  item->setParentItem(parentItem);
  item->setParent(parentItem);
}
