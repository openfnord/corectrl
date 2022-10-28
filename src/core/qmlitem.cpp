// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "qmlitem.h"

#include "iqmlcomponentfactory.h"
#include <Qt>

QString const &QMLItem::name() const
{
  return name_;
}

void QMLItem::setName(QString const &name)
{
  if (name_ != name) {
    name_ = name;
    emit nameChanged();
  }
}

void QMLItem::setupChild(QQuickItem *child)
{
  auto qmlItemChild = dynamic_cast<QMLItem *>(child);
  if (qmlItemChild != nullptr)
    connect(qmlItemChild, &QMLItem::settingsChanged, this,
            &QMLItem::settingsChanged, Qt::UniqueConnection);
}

QMLItem::Initializer::Initializer(IQMLComponentFactory const &qmlComponentFactory,
                                  QQmlApplicationEngine &qmlEngine) noexcept
: qmlComponentFactory_(qmlComponentFactory)
, qmlEngine_(qmlEngine)
{
}

std::pair<std::optional<std::reference_wrapper<Exportable::Exporter>>, QMLItem *>
QMLItem::Initializer::initializer(std::string const &itemID, QMLItem *parent)
{
  auto item = qmlComponentFactory_.createQMLItem(itemID, parent, qmlEngine_);
  if (item != nullptr) {
    auto factory = item->initializer(qmlComponentFactory_, qmlEngine_);
    if (factory != nullptr) {
      initializers_.emplace_back(std::move(factory));
      return {*initializers_.back(), item};
    }
  }

  return {};
}
