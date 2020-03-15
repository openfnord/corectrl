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
#pragma once

#include "helperids.h"
#include <KAuth>
#include <QEventLoop>
#include <QObject>
#include <QTimer>
#include <QVariantMap>
#include <memory>
#include <thread>

using namespace KAuth;

class ICryptoLayer;
class IAppRegistry;
class IProcessMonitor;
class MsgReceiver;
class QByteArray;

class Helper final : public QObject
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", DBUS_HELPER_INTERFACE)

 public:
  ~Helper();

 public slots: // KAuth init method
  ActionReply init(QVariantMap const &args);

 public slots: // D-Bus interface slots
  Q_SCRIPTABLE void exit(QByteArray const &signature);
  Q_SCRIPTABLE void delayAutoExit();

 private slots:
  void exitHelper();
  void autoExitTimeout();

 private:
  bool initCrypto(QByteArray const &appPublicKey);
  bool initDBus();
  void endDBus();
  bool initProcessMonitor();
  void endProcessMonitor();
  bool initMsgReceiver();

  QEventLoop eventLoop_;
  QTimer autoExitTimer_;

  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  std::shared_ptr<IAppRegistry> appRegistry_;
  std::unique_ptr<IProcessMonitor> processMonitor_;
  std::unique_ptr<std::thread> pMonThread_;
  std::unique_ptr<MsgReceiver> msgReceiver_;
};
