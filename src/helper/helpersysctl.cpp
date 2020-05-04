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
  QByteArray data;
  ctlCmds.pack(data);

  if (!data.isEmpty()) {
    auto signature = cryptoLayer_->signature(data);
    sysCtlInterface_->asyncCall(QStringLiteral("apply"), data, signature);
  }
}
