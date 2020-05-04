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
#include "helpercontrol.h"

#include "common/icryptolayer.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "helperids.h"
#include <KAuth>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QEventLoop>
#include <QString>
#include <QVariantMap>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

HelperControl::HelperControl(std::shared_ptr<ICryptoLayer> cryptoLayer,
                             QObject *parent) noexcept
: QObject(parent)
, cryptoLayer_(std::move(cryptoLayer))
{
  connect(&deferHelperAutoExitSignalTimer_, &QTimer::timeout, this,
          &HelperControl::helperExitDeferrerTimeout);
}

units::time::millisecond_t HelperControl::minExitTimeout() const
{
  return units::time::millisecond_t(1000);
}

void HelperControl::init(units::time::millisecond_t autoExitTimeout)
{
  autoExitTimeout = std::max(autoExitTimeout, minExitTimeout());
  units::time::millisecond_t deferAutoExitSignalInterval(autoExitTimeout * .75);

  cryptoLayer_->init();
  auto helperPublicKey = startHelper(autoExitTimeout,
                                     deferAutoExitSignalInterval);
  cryptoLayer_->usePublicKey(helperPublicKey);
}

void HelperControl::stop()
{
  deferHelperAutoExitSignalTimer_.stop();
  stopHelper();
}

void HelperControl::helperExitDeferrerTimeout()
{
  helperInterface_->asyncCall(QStringLiteral("delayAutoExit"));
}

bool HelperControl::isHelperRunning() const
{
  QDBusInterface iface(QStringLiteral(DBUS_HELPER_SERVICE),
                       QStringLiteral(DBUS_HELPER_PATH),
                       QStringLiteral("org.freedesktop.DBus.Introspectable"),
                       QDBusConnection::systemBus());
  if (!iface.isValid())
    return false;

  QDBusReply<void> reply = iface.call(QStringLiteral("Introspect"));
  if (reply.isValid())
    return true;

  return false;
}

void HelperControl::createHelperInterface()
{
  helperInterface_ = std::make_unique<QDBusInterface>(
      QStringLiteral(DBUS_HELPER_SERVICE), QStringLiteral(DBUS_HELPER_PATH),
      QStringLiteral(DBUS_HELPER_INTERFACE), QDBusConnection::systemBus());

  if (!helperInterface_->isValid())
    throw std::runtime_error(
        fmt::format("Cannot connect to DBus interface {} (path: {})",
                    DBUS_HELPER_INTERFACE, DBUS_HELPER_PATH));
}

QByteArray
HelperControl::startHelper(units::time::millisecond_t autoExitTimeout,
                           units::time::millisecond_t deferAutoExitSignalInterval)
{
  if (isHelperRunning()) {
    LOG(WARNING) << "Helper instance detected. Killing it now.";

    if (!killOtherHelper() || isHelperRunning())
      throw std::runtime_error("Failed to kill other helper instance");
  }

  QVariantMap args;
  args.insert(QStringLiteral("pubkey"), cryptoLayer_->publicKey());
  args.insert(QStringLiteral("autoExitTimeout"), autoExitTimeout.to<int>());

  KAuth::Action initAction(QStringLiteral(KAUTH_HELPER_ACTION));
  initAction.setHelperId(QStringLiteral(KAUTH_HELPER_ID));
  initAction.setTimeout(std::numeric_limits<int>::max());
  initAction.setArguments(args);

  KAuth::ExecuteJob *job = initAction.execute();

  bool success = true;
  QEventLoop waitForHelper;

  // setup helper ready signal handling
  QByteArray helperPublicKey;
  QMetaObject::Connection readySignalConnection = QObject::connect(
      job, &KAuth::ExecuteJob::newData, [&](QVariantMap const &data) {
        helperPublicKey = data[QStringLiteral("pubkey")].toByteArray();

        QObject::disconnect(readySignalConnection);
        waitForHelper.exit();
      });

  // setup helper init handling
  QObject::connect(job, &KJob::finished, [&]() {
    if (job->error()) {
      success = false;
      LOG(ERROR) << fmt::format("Helper start error. {}",
                                job->errorString().toStdString());
    }
    else
      success = job->data()[QStringLiteral("success")].toBool();

    if (!success)
      waitForHelper.exit();
  });

  job->start();
  waitForHelper.exec();

  if (!success)
    throw std::runtime_error("Cannot start helper");

  createHelperInterface();

  deferHelperAutoExitSignalTimer_.setInterval(
      deferAutoExitSignalInterval.to<int>());
  deferHelperAutoExitSignalTimer_.start();

  return helperPublicKey;
}

void HelperControl::stopHelper()
{
  auto signature = cryptoLayer_->signature({"exit"});
  helperInterface_->asyncCall(QStringLiteral("exit"), signature);
}

bool HelperControl::killOtherHelper() const
{
  KAuth::Action helperKillerAction(QStringLiteral(KAUTH_HELPER_KILLER_ACTION));
  helperKillerAction.setHelperId(QStringLiteral(KAUTH_HELPER_KILLER_ID));

  KAuth::ExecuteJob *job = helperKillerAction.execute();

  bool success = true;
  QEventLoop waitForHelper;

  // setup helper killer init handling
  QObject::connect(job, &KJob::finished, [&]() {
    if (job->error()) {
      success = false;
      LOG(ERROR) << fmt::format("Helper killer start error. {}",
                                job->errorString().toStdString());
    }
    else
      success = job->data()[QStringLiteral("success")].toBool();

    waitForHelper.exit();
  });

  job->start();
  waitForHelper.exec();

  return success;
}
