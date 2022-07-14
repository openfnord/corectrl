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
#pragma once

#include "helperids.h"
#include <QDBusAbstractAdaptor>
#include <QObject>

class QDBusMessage;

class HelperKiller final : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", DBUS_HELPER_KILLER_INTERFACE)
 public:
  HelperKiller(QObject *parent) noexcept;
  ~HelperKiller();

 public slots: // D-Bus interface slots
  bool start(QDBusMessage const &message) const;

 private:
  bool isAuthorized(QDBusMessage const &message) const;
};
