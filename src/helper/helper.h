// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
