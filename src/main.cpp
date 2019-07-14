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
#include "app/app.h"
#include "app/appfactory.h"
#include "common/logger.h"
#include <QCoreApplication>
#include <signal.h>

INITIALIZE_EASYLOGGINGPP

// Handle easily unix signals
//
// Credits: Amir Zamani
// https://gist.github.com/azadkuh/a2ac6869661ebd3f8588#file-qt-unix-signals-md
void catchUnixSignals(std::initializer_list<int> quitSignals)
{
  auto handler = [](int) -> void {
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit",
                              Qt::QueuedConnection);
  };

  sigset_t blocking_mask;
  sigemptyset(&blocking_mask);
  for (auto sig : quitSignals)
    sigaddset(&blocking_mask, sig);

  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_mask = blocking_mask;
  sa.sa_flags = 0;

  for (auto sig : quitSignals)
    sigaction(sig, &sa, nullptr);
}

int main(int argc, char **argv)
{
  setupLogger(std::filesystem::temp_directory_path() /
              (std::string(App::Name) + ".log"));
  START_EASYLOGGINGPP(argc, argv);

  LOG(INFO) << "----- Application started -----";

  catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});

  AppFactory appFactory;
  auto app = appFactory.build();
  if (app != nullptr)
    return app->exec(argc, argv);
  else
    return -1;
}
