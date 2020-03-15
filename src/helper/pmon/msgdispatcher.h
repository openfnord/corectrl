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
#include "ipmonmsgdispatcher.h"
#include <QObject>
#include <memory>

class IAppRegistry;
class ICryptoLayer;
class QByteArray;

// Methods of PMonMsgDispatcher class will be called in the ProcessMonitor
// thread. However D-Bus signals must be emitted in the thread of the object
// that registered the D-Bus object. For this reason, signals are forwarded
// to PMonDBusSignalDispatcher through QObject signal / slot mechanism.
// PMonDBusSignalDispatcher, as the object that registered the D-Bus object,
// will re-emit the forwaded signals through D-Bus in the event loop.

class DBusSignalDispatcher final : public QObject
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", DBUS_HELPER_PMON_INTERFACE)

 public:
  DBusSignalDispatcher(std::shared_ptr<ICryptoLayer> cryptoLayer,
                       std::shared_ptr<IAppRegistry> appRegistry,
                       QObject *parent = nullptr);
  ~DBusSignalDispatcher() override;

 public slots:
  void sendExec(QByteArray const &data);
  void sendExit(QByteArray const &data);

  // D-Bus interface methods
  Q_SCRIPTABLE void watchApp(QByteArray const &data, QByteArray const &signature);
  Q_SCRIPTABLE void forgetApp(QByteArray const &data,
                              QByteArray const &signature);

 signals:
  // D-Bus interface signals
  Q_SCRIPTABLE void appExec(QByteArray const &data, QByteArray const &signature);
  Q_SCRIPTABLE void appExit(QByteArray const &data, QByteArray const &signature);

 private:
  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  std::shared_ptr<IAppRegistry> appRegistry_;
};

class MsgDispatcher final
: public QObject
, public IPMonMsgDispatcher
{
  Q_OBJECT

 public:
  MsgDispatcher(std::shared_ptr<ICryptoLayer> cryptoLayer,
                std::shared_ptr<IAppRegistry> appRegistry,
                QObject *parent = nullptr);

  void sendExec(std::string const &app) override;
  void sendExit(std::string const &app) override;

 signals:
  void appExec(QByteArray const &data);
  void appExit(QByteArray const &data);

 private:
  DBusSignalDispatcher dbusSignalDispatcher_;
};
