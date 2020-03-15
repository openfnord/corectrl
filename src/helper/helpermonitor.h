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
