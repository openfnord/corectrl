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

#include <QByteArray>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QString>
#include <QStringList>
#include <string_view>

class SingleInstance : public QObject
{
  Q_OBJECT
 public:
  explicit SingleInstance(std::string_view name, QObject *parent = 0);

  bool mainInstance(QStringList const &args);

 signals:
  void newInstance(QStringList args);

 private slots:
  void newConnection();

 private:
  QByteArray toRawData(QStringList const &data) const;

  QString const name_;
  QLocalServer server_;
};

class SingleInstanceClient : public QObject
{
  Q_OBJECT
 public:
  SingleInstanceClient(QLocalSocket *client);

 signals:
  void newInstance(QStringList args);

 public slots:
  void onReadyRead();
  void onDisconnected();

 private:
  QStringList fromRawData(QByteArray const &data) const;

  QStringList args_;
};
