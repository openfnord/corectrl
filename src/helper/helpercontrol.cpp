// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "helpercontrol.h"

#include "common/icryptolayer.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "helperids.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusVariant>
#include <QString>
#include <QVariant>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

HelperControl::HelperControl(std::shared_ptr<ICryptoLayer> cryptoLayer,
                             QObject *parent) noexcept
: QObject(parent)
, cryptoLayer_(std::move(cryptoLayer))
, autoExitTimeout_(minExitTimeout())
, deferAutoExitSignalInterval_(minExitTimeout() * .667)
{
  connect(&deferHelperHealthCheckTimer_, &QTimer::timeout, this,
          &HelperControl::helperHealthCheckTimeout);
  connect(&deferHelperAutoExitSignalTimer_, &QTimer::timeout, this,
          &HelperControl::helperExitDeferrerTimeout);
}

units::time::millisecond_t HelperControl::minExitTimeout() const
{
  return units::time::millisecond_t(1000);
}

void HelperControl::init(units::time::millisecond_t autoExitTimeout)
{
  autoExitTimeout_ = std::max(autoExitTimeout, minExitTimeout());
  deferAutoExitSignalInterval_ = autoExitTimeout * .667;

  cryptoLayer_->init();
  createHelperInterface();

  killOtherHelperInstance();

  auto helperPublicKey = startHelper();
  if (!helperPublicKey.has_value())
    throw std::runtime_error("Cannot start helper");

  cryptoLayer_->usePublicKey(helperPublicKey.value());

  // Check the helper health every 15 seconds.
  deferHelperHealthCheckTimer_.setInterval(15000);
  deferHelperHealthCheckTimer_.start();
}

void HelperControl::stop()
{
  deferHelperHealthCheckTimer_.stop();
  deferHelperAutoExitSignalTimer_.stop();
  stopHelper();
}

void HelperControl::helperHealthCheckTimeout()
{
  if (deferHelperAutoExitSignalTimer_.isActive() && !helperHasBeenStarted()) {
    LOG(WARNING) << "The Helper has not been started. Starting it now.";

    deferHelperAutoExitSignalTimer_.stop();
    auto helperPublicKey = startHelper();

    if (!helperPublicKey.has_value()) {
      // NOTE If the helper died and cannot be started again, it's better to not
      // crash the application throwing an exception.
      LOG(WARNING) << "Cannot restart helper!";
    }

    cryptoLayer_->usePublicKey(helperPublicKey.value());
  }
}

void HelperControl::helperExitDeferrerTimeout()
{
  helperInterface_->asyncCall(QStringLiteral("delayAutoExit"));
}

bool HelperControl::helperHasBeenStarted() const
{
  QDBusReply<bool> reply = helperInterface_->call(QStringLiteral("started"));
  return reply.isValid() && reply.value();
}

void HelperControl::createHelperInterface()
{
  helperInterface_ = std::make_unique<QDBusInterface>(
      QStringLiteral(DBUS_HELPER_SERVICE), QStringLiteral(DBUS_HELPER_PATH),
      QStringLiteral(DBUS_HELPER_INTERFACE), QDBusConnection::systemBus());

  if (!helperInterface_->isValid())
    throw std::runtime_error(fmt::format(
        "Cannot connect to D-Bus interface {}: {}", DBUS_HELPER_INTERFACE,
        helperInterface_->lastError().message().toStdString()));
}

std::optional<QByteArray> HelperControl::startHelper()
{
  QDBusReply<QDBusVariant> reply = helperInterface_->call(
      QStringLiteral("start"), cryptoLayer_->publicKey(),
      autoExitTimeout_.to<int>());

  if (!(reply.isValid() && reply.value().variant().type() == QVariant::ByteArray))
    return std::nullopt;

  deferHelperAutoExitSignalTimer_.setInterval(
      deferAutoExitSignalInterval_.to<int>());
  deferHelperAutoExitSignalTimer_.start();

  return reply.value().variant().value<QByteArray>();
}

void HelperControl::stopHelper()
{
  auto signature = cryptoLayer_->signature({"exit"});
  helperInterface_->asyncCall(QStringLiteral("exit"), signature);
}

void HelperControl::killOtherHelperInstance()
{
  if (helperHasBeenStarted()) {
    LOG(WARNING) << "Helper instance detected. Killing it now.";

    if (!startHelperKiller() || helperHasBeenStarted())
      throw std::runtime_error("Failed to kill other helper instance");
  }
}

bool HelperControl::startHelperKiller()
{
  QDBusInterface iface(QStringLiteral(DBUS_HELPER_KILLER_SERVICE),
                       QStringLiteral(DBUS_HELPER_KILLER_PATH),
                       QStringLiteral(DBUS_HELPER_KILLER_INTERFACE),
                       QDBusConnection::systemBus());
  if (!iface.isValid()) {
    LOG(ERROR) << fmt::format("Cannot connect to D-Bus interface {}: {}",
                              DBUS_HELPER_KILLER_INTERFACE,
                              iface.lastError().message().toStdString());
    return false;
  }

  QDBusReply<bool> reply = iface.call(QStringLiteral("start"));
  if (!reply.isValid()) {
    LOG(ERROR) << fmt::format("Helper killer error: {}",
                              iface.lastError().message().toStdString());
    return false;
  }

  return reply.value();
}
