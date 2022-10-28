// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
