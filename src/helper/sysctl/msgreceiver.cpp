// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "msgreceiver.h"

#include "common/icryptolayer.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "isysfswriter.h"
#include <QByteArray>
#include <QDBusConnection>
#include <QDBusError>
#include <QString>
#include <stdexcept>
#include <utility>

MsgReceiver::MsgReceiver(std::shared_ptr<ICryptoLayer> cryptoLayer,
                         std::unique_ptr<ISysfsWriter> &&sysfsWriter,
                         QObject *parent)
: QObject(parent)
, cryptoLayer_(std::move(cryptoLayer))
, sysfsWriter_(std::move(sysfsWriter))
{
  QDBusConnection dbusConnection = QDBusConnection::systemBus();
  if (!dbusConnection.isConnected())
    throw std::runtime_error("Could not connect to D-Bus system bus");

  if (!dbusConnection.registerObject(
          QStringLiteral(DBUS_HELPER_SYSCTL_PATH),
          QStringLiteral(DBUS_HELPER_SYSCTL_INTERFACE), this,
          QDBusConnection::ExportScriptableSlots))
    throw std::runtime_error(
        fmt::format("Could not register D-Bus object on path {} "
                    "using the interface {}\n.Last D-Bus error: {}",
                    DBUS_HELPER_SYSCTL_PATH, DBUS_HELPER_SYSCTL_INTERFACE,
                    dbusConnection.lastError().message().toStdString()));
}

MsgReceiver::~MsgReceiver()
{
  QDBusConnection dbusConnection = QDBusConnection::systemBus();
  dbusConnection.unregisterObject(QStringLiteral(DBUS_HELPER_SYSCTL_INTERFACE));
}

void MsgReceiver::apply(QByteArray const &data, QByteArray const &signature)
{
  if (cryptoLayer_->verify(data, signature)) {
    auto cmdList = data.split('\0');
    for (int i = 0; i + 1 < cmdList.size(); i += 2)
      sysfsWriter_->write(cmdList[i].data(), cmdList[i + 1].data());
  }
  else
    LOG(ERROR) << "Failed to verify received data from D-Bus";
}
