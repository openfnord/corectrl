// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "helpersysctl.h"

#include "common/icryptolayer.h"
#include "core/icommandqueue.h"
#include "fmt/format.h"
#include "helperids.h"
#include <QByteArray>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QString>
#include <stdexcept>
#include <utility>

HelperSysCtl::HelperSysCtl(std::shared_ptr<ICryptoLayer> cryptoLayer) noexcept
: cryptoLayer_(std::move(cryptoLayer))
{
}

void HelperSysCtl::init()
{
  sysCtlInterface_ = std::make_unique<QDBusInterface>(
      QStringLiteral(DBUS_HELPER_SERVICE),
      QStringLiteral(DBUS_HELPER_SYSCTL_PATH),
      QStringLiteral(DBUS_HELPER_SYSCTL_INTERFACE),
      QDBusConnection::systemBus());

  if (!sysCtlInterface_->isValid()) {
    throw std::runtime_error(
        fmt::format("Cannot connect to D-Bus interface {} (path: {})",
                    DBUS_HELPER_SYSCTL_INTERFACE, DBUS_HELPER_SYSCTL_PATH));
  }
}

void HelperSysCtl::apply(ICommandQueue &ctlCmds)
{
  auto data = ctlCmds.toRawData();
  if (!data.isEmpty()) {
    auto signature = cryptoLayer_->signature(data);
    sysCtlInterface_->asyncCall(QStringLiteral("apply"), data, signature);
  }
}
