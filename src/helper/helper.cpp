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
#include "sysctl/msgreceiver.h"
#include "sysctl/sysfswriter.h"
#include <QByteArray>
#include <QDBusConnection>
#include <QDBusError>
#include <QString>
#include <QVariantMap>
#include <exception>
#include <filesystem>

INITIALIZE_EASYLOGGINGPP

Helper::~Helper() = default;

ActionReply Helper::init(QVariantMap const &args)
{
  setupLogger(std::filesystem::temp_directory_path() / LOG_HELPER_FILE_NAME, "1");

  LOG(INFO) << "----- Helper started -----";

  ActionReply reply;

  auto appPublicKey = args[QStringLiteral("pubkey")].toByteArray();
  if (!initCrypto(appPublicKey)) {
    reply.addData(QStringLiteral("success"), false);
    return reply;
  }

  if (!initDBus()) {
    reply.addData(QStringLiteral("success"), false);
    return reply;
  }

  if (!(initProcessMonitor() && initMsgReceiver())) {
    endDBus();

    reply.addData(QStringLiteral("success"), false);
    return reply;
  }

  auto const autoExitTime = args[QStringLiteral("autoExitTimeout")].toInt();
  autoExitTimer_.setInterval(autoExitTime);
  autoExitTimer_.setSingleShot(true);
  connect(&autoExitTimer_, &QTimer::timeout, this, &Helper::autoExitTimeout);
  autoExitTimer_.start();

  // send ready signal to main app
  QVariantMap readyArgs;
  readyArgs.insert(QStringLiteral("pubkey"), cryptoLayer_->publicKey());
  HelperSupport::progressStep(readyArgs);

  eventLoop_.exec();

  reply.addData(QStringLiteral("success"), true);
  return reply;
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
  endDBus();

  eventLoop_.exit();
}

void Helper::autoExitTimeout()
{
  LOG(WARNING) << "Auto exit timeout. Killing helper instance...";
  exitHelper();
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
  autoExitTimer_.start();
}

bool Helper::initDBus()
{
  QDBusConnection dbusConnection = QDBusConnection::systemBus();
  if (!dbusConnection.isConnected()) {
    LOG(ERROR) << "Could not connect to D-Bus system bus";
    return false;
  }

  if (!dbusConnection.registerService(QStringLiteral(DBUS_HELPER_SERVICE))) {
    LOG(ERROR) << fmt::format(
        "Could not register D-Bus service {}.\nLast D-Bus error: {}",
        DBUS_HELPER_SERVICE, dbusConnection.lastError().message().toStdString());
    return false;
  }

  if (!dbusConnection.registerObject(QStringLiteral(DBUS_HELPER_PATH),
                                     QStringLiteral(DBUS_HELPER_INTERFACE), this,
                                     QDBusConnection::ExportScriptableSignals |
                                         QDBusConnection::ExportScriptableSlots)) {
    LOG(ERROR) << fmt::format("Could not register D-Bus object on path {} "
                              "using the interface {}\n.Last D-Bus error: {}",
                              DBUS_HELPER_PATH, DBUS_HELPER_INTERFACE,
                              dbusConnection.lastError().message().toStdString());
    return false;
  }

  return true;
}

void Helper::endDBus()
{
  QDBusConnection dbusConnection = QDBusConnection::systemBus();
  if (!dbusConnection.unregisterService(QStringLiteral(DBUS_HELPER_SERVICE))) {
    LOG(ERROR) << fmt::format(
        "D-Bus error unregistering service {}: {}", DBUS_HELPER_SERVICE,
        dbusConnection.lastError().message().toStdString());
  }

  dbusConnection.unregisterObject(QStringLiteral(DBUS_HELPER_PATH));
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
  processMonitor_->stop();
  pMonThread_->join();
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

KAUTH_HELPER_MAIN(KAUTH_HELPER_ID, Helper)
