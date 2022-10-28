// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
