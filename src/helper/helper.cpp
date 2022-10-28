// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "helper.h"

#include "common/cryptolayer.h"
#include "common/logger.h"
#include "fmt/format.h"
#include "helperids.h"
#include "pmon/appregistry.h"
#include "pmon/msgdispatcher.h"
#include "pmon/nlprocexecmonitor.h"
#include "pmon/nlprocexecsocket.h"
#include "pmon/procpidsolver.h"
#include "polkit.h"
#include "sysctl/msgreceiver.h"
#include "sysctl/sysfswriter.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QString>
#include <exception>
#include <filesystem>

INITIALIZE_EASYLOGGINGPP

Helper::Helper(QObject *parent) noexcept
: QDBusAbstractAdaptor(parent)
{
}

Helper::~Helper() = default;

QDBusVariant Helper::start(QByteArray const &appPublicKey, int autoExitTimeout,
                           QDBusMessage const &message)
{
  if (!started()) {
    setupLogger(std::filesystem::temp_directory_path() / LOG_HELPER_FILE_NAME,
                "1");

    LOG(INFO) << "----- Helper started -----";

    if (!(isAuthorized(message) && initCrypto(appPublicKey) &&
          initProcessMonitor() && initMsgReceiver())) {
      exitHelper();
      return QDBusVariant(false);
    }

    autoExitTimer_.setInterval(autoExitTimeout);
    autoExitTimer_.setSingleShot(true);
    connect(&autoExitTimer_, &QTimer::timeout, this, &Helper::autoExitTimeout);
    autoExitTimer_.start();
  }

  return QDBusVariant(cryptoLayer_->publicKey());
}

bool Helper::started() const
{
  return autoExitTimer_.isActive();
}

void Helper::exit(QByteArray const &signature)
{
  if (cryptoLayer_->verify({"exit"}, signature)) {
    autoExitTimer_.stop();
    exitHelper();
  }
  else
    LOG(ERROR) << "Failed to verify 'exit' call from D-Bus";
}

void Helper::exitHelper()
{
  endProcessMonitor();
  QTimer::singleShot(0, QCoreApplication::instance(), &QCoreApplication::quit);
}

void Helper::autoExitTimeout()
{
  LOG(WARNING) << "Auto exit timeout. Killing helper instance...";
  exitHelper();
}

bool Helper::isAuthorized(QDBusMessage const &message) const
{
  auto subject = Polkit::BusNameSubject{message.service().toStdString()};
  return Polkit::checkAuthorizationSync(POLKIT_HELPER_ACTION, subject) ==
         Polkit::AuthResult::Yes;
}

bool Helper::initCrypto(QByteArray const &appPublicKey)
{
  try {
    cryptoLayer_ = std::make_shared<CryptoLayer>();
    cryptoLayer_->init();
    cryptoLayer_->usePublicKey(appPublicKey);

    return true;
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  return false;
}

void Helper::delayAutoExit()
{
  if (autoExitTimer_.isActive())
    autoExitTimer_.start();
}

bool Helper::initProcessMonitor()
{
  bool success = false;

  try {
    appRegistry_ = std::make_shared<AppRegistry>();
    processMonitor_ = std::make_unique<NLProcExecMonitor>(
        appRegistry_, std::make_unique<ProcPIDSolver>(),
        std::make_unique<MsgDispatcher>(cryptoLayer_, appRegistry_));

    auto pMonInit = [&]() { processMonitor_->start(); };
    pMonThread_ = std::make_unique<std::thread>(pMonInit);

    success = true;
  }
  catch (NLProcExecSocket::BindError &e) {
    LOG(WARNING) << "Bind socket error: " << e.what();
    LOG(WARNING) << "Do you have root permissions?";
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  return success;
}

void Helper::endProcessMonitor()
{
  if (pMonThread_ != nullptr) {
    processMonitor_->stop();
    pMonThread_->join();
  }
}

bool Helper::initMsgReceiver()
{
  try {
    msgReceiver_ = std::make_unique<MsgReceiver>(
        cryptoLayer_, std::make_unique<SysfsWriter>());

    return true;
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  return false;
}

bool initDBusForHelperService(QObject *obj)
{
  QDBusConnection bus = QDBusConnection::systemBus();
  if (!bus.isConnected()) {
    LOG(ERROR) << "Could not connect to D-Bus system bus";
    return false;
  }

  if (!bus.registerObject(QStringLiteral(DBUS_HELPER_PATH), obj)) {
    LOG(ERROR) << fmt::format("Could not register D-Bus object on path {} "
                              "using the interface {}\n.Last D-Bus error: {}",
                              DBUS_HELPER_PATH, DBUS_HELPER_INTERFACE,
                              bus.lastError().message().toStdString());
    return false;
  }

  if (!bus.registerService(QStringLiteral(DBUS_HELPER_SERVICE))) {
    LOG(ERROR) << fmt::format(
        "Could not register D-Bus service {}.\nLast D-Bus error: {}",
        DBUS_HELPER_SERVICE, bus.lastError().message().toStdString());
    return false;
  }

  return true;
}

bool endDBusForHelperService()
{
  QDBusConnection bus = QDBusConnection::systemBus();
  bus.unregisterObject(QStringLiteral(DBUS_HELPER_PATH));
  auto success = bus.unregisterService(QStringLiteral(DBUS_HELPER_SERVICE));
  if (!success) {
    LOG(ERROR) << fmt::format("D-Bus error unregistering service {}: {}",
                              DBUS_HELPER_SERVICE,
                              bus.lastError().message().toStdString());
  }

  return success;
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  new Helper(&app);

  if (!initDBusForHelperService(&app))
    exit(1);

  app.exec();

  if (!endDBusForHelperService())
    exit(1);

  return 0;
}
