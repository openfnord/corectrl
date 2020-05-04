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
#include "helpermonitor.h"

#include "common/icryptolayer.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "helperids.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QString>
#include <algorithm>
#include <stdexcept>
#include <utility>

HelperMonitor::HelperMonitor(std::shared_ptr<ICryptoLayer> cryptoLayer,
                             QObject *parent) noexcept
: QObject(parent)
, cryptoLayer_(std::move(cryptoLayer))
{
}

void HelperMonitor::addObserver(std::shared_ptr<IHelperMonitor::Observer> observer)
{
  std::lock_guard<std::mutex> lock(mutex_);

  auto const it = std::find(observers_.begin(), observers_.end(), observer);
  if (it == observers_.end())
    observers_.emplace_back(std::move(observer));
}

void HelperMonitor::removeObserver(
    std::shared_ptr<IHelperMonitor::Observer> const &observer)
{
  std::lock_guard<std::mutex> lock(mutex_);
  observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                   observers_.end());
}

void HelperMonitor::init()
{
  monitorInterface_ = std::make_unique<QDBusInterface>(
      QStringLiteral(DBUS_HELPER_SERVICE), QStringLiteral(DBUS_HELPER_PMON_PATH),
      QStringLiteral(DBUS_HELPER_PMON_INTERFACE), QDBusConnection::systemBus());

  if (!monitorInterface_->isValid())
    throw std::runtime_error(
        fmt::format("Cannot connect to D-Bus interface {} (path: {})",
                    DBUS_HELPER_PMON_INTERFACE, DBUS_HELPER_PMON_PATH));

  if (!QDBusConnection::systemBus().connect(
          QStringLiteral(DBUS_HELPER_SERVICE),
          QStringLiteral(DBUS_HELPER_PMON_PATH),
          QStringLiteral(DBUS_HELPER_PMON_INTERFACE), QStringLiteral("appExec"),
          this, SLOT(notifyAppExec(QByteArray const &, QByteArray const &))))
    throw std::runtime_error(fmt::format(
        "Cannot connect to 'appExec' in D-Bus interface {} (path: {})",
        DBUS_HELPER_PMON_INTERFACE, DBUS_HELPER_PMON_PATH));

  if (!QDBusConnection::systemBus().connect(
          QStringLiteral(DBUS_HELPER_SERVICE),
          QStringLiteral(DBUS_HELPER_PMON_PATH),
          QStringLiteral(DBUS_HELPER_PMON_INTERFACE), QStringLiteral("appExit"),
          this, SLOT(notifyAppExit(QByteArray const &, QByteArray const &))))
    throw std::runtime_error(fmt::format(
        "Cannot connect to 'appExit' in D-Bus interface {} (path: {})",
        DBUS_HELPER_PMON_INTERFACE, DBUS_HELPER_PMON_PATH));
}

void HelperMonitor::watchApp(std::string const &app)
{
  QByteArray data(app.c_str());
  auto signature = cryptoLayer_->signature(data);
  monitorInterface_->asyncCall(QStringLiteral("watchApp"), data, signature);
}

void HelperMonitor::forgetApp(std::string const &app)
{
  QByteArray data(app.c_str());
  auto signature = cryptoLayer_->signature(data);
  monitorInterface_->asyncCall(QStringLiteral("forgetApp"), data, signature);
}

void HelperMonitor::notifyAppExec(QByteArray const &data,
                                  QByteArray const &signature)
{
  if (!cryptoLayer_->verify(data, signature)) {
    LOG(ERROR) << "Failed to verify received data from D-Bus";
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  auto const app = data.toStdString();
  for (auto &o : observers_)
    o->appExec(app);
}

void HelperMonitor::notifyAppExit(QByteArray const &data,
                                  QByteArray const &signature)
{
  if (!cryptoLayer_->verify(data, signature)) {
    LOG(ERROR) << "Failed to verify received data from D-Bus";
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  auto const app = data.toStdString();
  for (auto &o : observers_)
    o->appExit(app);
}
