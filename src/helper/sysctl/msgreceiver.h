// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "helperids.h"
#include <QObject>
#include <memory>

class ICryptoLayer;
class ISysfsWriter;
class QByteArray;

class MsgReceiver final : public QObject
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", DBUS_HELPER_SYSCTL_INTERFACE)

 public:
  MsgReceiver(std::shared_ptr<ICryptoLayer> cryptoLayer,
              std::unique_ptr<ISysfsWriter> &&sysfsWriter,
              QObject *parent = nullptr);
  ~MsgReceiver();

 public slots:
  // D-Bus interface methods
  Q_SCRIPTABLE void apply(QByteArray const &data, QByteArray const &signature);

 private:
  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  std::unique_ptr<ISysfsWriter> sysfsWriter_;
};
