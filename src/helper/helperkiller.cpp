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

#include "helperids.h"
#include <QProcess>
#include <QString>
#include <exception>
#include <signal.h>
#include <string>

HelperKiller::~HelperKiller() = default;

ActionReply HelperKiller::init(QVariantMap const &)
{
  QString cmdStr("pidof ");
  cmdStr.append(HELPER_EXE);

  QProcess cmd;
  cmd.start(cmdStr);

  bool success = cmd.waitForFinished();
  if (success) {
    try {
      auto output = cmd.readAllStandardOutput().toStdString();
      int pid = std::stoi(output);
      success = kill(pid, SIGKILL) == 0;
    }
    catch (std::exception const &) {
      success = false;
    }
  }

  ActionReply reply;
  reply.addData(QStringLiteral("success"), success);
  return reply;
}

KAUTH_HELPER_MAIN(KAUTH_HELPER_KILLER_ID, HelperKiller)
