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
    qmlRegisterType<AMD::PMOverclockQMLItem>("Radman.UIComponents", 1, 0,
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
