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
#include "msgdispatcher.h"

#include "common/icryptolayer.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "iappregistry.h"
#include <QByteArray>
#include <QDBusConnection>
#include <QDBusError>
#include <QString>
#include <stdexcept>
#include <utility>

DBusSignalDispatcher::DBusSignalDispatcher(
    std::shared_ptr<ICryptoLayer> cryptoLayer,
    std::shared_ptr<IAppRegistry> appRegistry, QObject *parent)
: QObject(parent)
, cryptoLayer_(std::move(cryptoLayer))
, appRegistry_(std::move(appRegistry))
{
  QDBusConnection dbusConnection = QDBusConnection::systemBus();
  if (!dbusConnection.isConnected())
    throw std::runtime_error("Could not connect to D-Bus system bus");

  if (!dbusConnection.registerObject(QStringLiteral(DBUS_HELPER_PMON_PATH),
                                     QStringLiteral(DBUS_HELPER_PMON_INTERFACE),
                                     this,
                                     QDBusConnection::ExportScriptableSignals |
                                         QDBusConnection::ExportScriptableSlots))
    throw std::runtime_error(
        fmt::format("Could not register D-Bus object on path {} "
                    "using the interface {}\n.Last D-Bus error: {}",
                    DBUS_HELPER_PMON_PATH, DBUS_HELPER_PMON_INTERFACE,
                    dbusConnection.lastError().message().toStdString()));
}

DBusSignalDispatcher::~DBusSignalDispatcher()
{
  QDBusConnection dbusConnection = QDBusConnection::systemBus();
  dbusConnection.unregisterObject(QStringLiteral(DBUS_HELPER_PMON_INTERFACE));
}

void DBusSignalDispatcher::sendExec(QByteArray const &data)
{
  auto signature = cryptoLayer_->signature(data);
  emit appExec(data, signature);
}

void DBusSignalDispatcher::sendExit(QByteArray const &data)
{
  auto signature = cryptoLayer_->signature(data);
  emit appExit(data, signature);
}

void DBusSignalDispatcher::watchApp(QByteArray const &data,
                                    QByteArray const &signature)
{
  if (cryptoLayer_->verify(data, signature))
    appRegistry_->add(data.toStdString());
  else
    LOG(ERROR) << "Failed to verify received data from D-Bus";
}

void DBusSignalDispatcher::forgetApp(QByteArray const &data,
                                     QByteArray const &signature)
{
  if (cryptoLayer_->verify(data, signature))
    appRegistry_->remove(data.toStdString());
  else
    LOG(ERROR) << "Failed to verify received data from D-Bus";
}

MsgDispatcher::MsgDispatcher(std::shared_ptr<ICryptoLayer> cryptoLayer,
                             std::shared_ptr<IAppRegistry> appRegistry,
                             QObject *parent)
: QObject(parent)
, dbusSignalDispatcher_(std::move(cryptoLayer), std::move(appRegistry), this)
{
  connect(this, &MsgDispatcher::appExec, &dbusSignalDispatcher_,
          &DBusSignalDispatcher::sendExec);
  connect(this, &MsgDispatcher::appExit, &dbusSignalDispatcher_,
          &DBusSignalDispatcher::sendExit);
}

void MsgDispatcher::sendExec(std::string const &app)
{
  emit appExec({app.c_str()});
}

void MsgDispatcher::sendExit(std::string const &app)
{
  emit appExit({app.c_str()});
}
