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
#include <QDBusAbstractAdaptor>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QObject>
#include <QTimer>
#include <memory>
#include <thread>

class ICryptoLayer;
class IAppRegistry;
class IProcessMonitor;
class MsgReceiver;
class QByteArray;

class Helper final : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", DBUS_HELPER_INTERFACE)

 public:
  Helper(QObject *parent) noexcept;
  ~Helper();

 public slots: // D-Bus interface slots
  QDBusVariant start(QByteArray const &appPublicKey, int autoExitTimeout,
                     QDBusMessage const &message);
  bool started() const;
  Q_NOREPLY void exit(QByteArray const &signature);
  Q_NOREPLY void delayAutoExit();

 private slots:
  void exitHelper();
  void autoExitTimeout();

 private:
  bool isAuthorized(QDBusMessage const &message) const;
  bool initCrypto(QByteArray const &appPublicKey);
  bool initProcessMonitor();
  void endProcessMonitor();
  bool initMsgReceiver();

  QTimer autoExitTimer_;

  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  std::shared_ptr<IAppRegistry> appRegistry_;
  std::unique_ptr<IProcessMonitor> processMonitor_;
  std::unique_ptr<std::thread> pMonThread_;
  std::unique_ptr<MsgReceiver> msgReceiver_;
};
