// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "ihelpermonitor.h"
#include <QByteArray>
#include <QObject>
#include <QtDBus>
#include <memory>
#include <mutex>
#include <vector>

class ICryptoLayer;

class HelperMonitor final
: public QObject
, public IHelperMonitor
{
  Q_OBJECT

 public:
  HelperMonitor(std::shared_ptr<ICryptoLayer> cryptoLayer,
                QObject *parent = nullptr) noexcept;

  void addObserver(std::shared_ptr<IHelperMonitor::Observer> observer) override;
  void removeObserver(
      std::shared_ptr<IHelperMonitor::Observer> const &observer) override;

  void init() override;

  void watchApp(std::string const &app) override;
  void forgetApp(std::string const &app) override;

 private slots:
  void notifyAppExec(QByteArray const &appExe, QByteArray const &signature);
  void notifyAppExit(QByteArray const &appExe, QByteArray const &signature);

 private:
  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  std::unique_ptr<QDBusInterface> monitorInterface_;
  std::vector<std::shared_ptr<IHelperMonitor::Observer>> observers_;
  std::mutex mutex_;
};
