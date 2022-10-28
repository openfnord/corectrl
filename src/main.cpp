// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
