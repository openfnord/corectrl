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
#include "helperkiller.h"

#include "easyloggingpp/easylogging++.h"
#include "helperids.h"
#include "polkit.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <exception>
#include <signal.h>
#include <string>

INITIALIZE_EASYLOGGINGPP

HelperKiller::HelperKiller(QObject *parent) noexcept
: QDBusAbstractAdaptor(parent)
{
}

HelperKiller::~HelperKiller() = default;

bool HelperKiller::start(QDBusMessage const &message) const
{
  if (!isAuthorized(message))
    return false;

  QProcess cmd;
  cmd.start(QStringLiteral("pidof"), QStringList(HELPER_EXE));

  bool result = cmd.waitForFinished();
  if (result) {
    try {
      auto output = cmd.readAllStandardOutput().toStdString();
      int pid = std::stoi(output);
      result = kill(pid, SIGKILL) == 0;
    }
    catch (std::exception const &) {
      result = false;
    }
  }

  QTimer::singleShot(0, QCoreApplication::instance(), &QCoreApplication::quit);

  return result;
}

bool HelperKiller::isAuthorized(QDBusMessage const &message) const
{
  auto subject = Polkit::BusNameSubject{message.service().toStdString()};
  return Polkit::checkAuthorizationSync(POLKIT_HELPER_KILLER_ACTION, subject) ==
         Polkit::AuthResult::Yes;
}

bool initDBusForHelperKillerService(QObject *obj)
{
  QDBusConnection bus = QDBusConnection::systemBus();
  return bus.isConnected() &&
         bus.registerObject(QStringLiteral(DBUS_HELPER_KILLER_PATH), obj) &&
         bus.registerService(QStringLiteral(DBUS_HELPER_KILLER_SERVICE));
}

bool endDBusForHelperKillerService()
{
  QDBusConnection bus = QDBusConnection::systemBus();
  bus.unregisterObject(QStringLiteral(DBUS_HELPER_KILLER_PATH));
  return bus.unregisterService(QStringLiteral(DBUS_HELPER_KILLER_SERVICE));
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  new HelperKiller(&app);

  if (!initDBusForHelperKillerService(&app))
    exit(1);

  app.exec();

  if (!endDBusForHelperKillerService())
    exit(1);

  return 0;
}
