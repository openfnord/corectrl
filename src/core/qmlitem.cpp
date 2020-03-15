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
