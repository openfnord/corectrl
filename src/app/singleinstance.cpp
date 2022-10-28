// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "singleinstance.h"

SingleInstanceClient::SingleInstanceClient(QLocalSocket *client)
: QObject(client)
{
  connect(client, &QLocalSocket::readyRead, this,
          &SingleInstanceClient::onReadyRead);
  connect(client, &QLocalSocket::disconnected, this,
          &SingleInstanceClient::onDisconnected);
  connect(client, &QLocalSocket::disconnected, client,
          &QLocalSocket::deleteLater);
}

void SingleInstanceClient::onReadyRead()
{
  QLocalSocket *client = qobject_cast<QLocalSocket *>(parent());
  args_ = fromRawData(client->readAll());
}

void SingleInstanceClient::onDisconnected()
{
  emit newInstance(args_);
}

QStringList SingleInstanceClient::fromRawData(QByteArray const &data) const
{
  QStringList result;

  auto dataList = data.split('\0');
  for (auto const &rawData : dataList)
    result.push_back(QString::fromUtf8(rawData));

  return result;
}

SingleInstance::SingleInstance(std::string_view name, QObject *parent)
: QObject(parent)
, name_(name.data())
{
  connect(&server_, &QLocalServer::newConnection, this,
          &SingleInstance::newConnection);
}

bool SingleInstance::mainInstance(QStringList const &args)
{
  QLocalSocket socket;
  socket.connectToServer(name_, QLocalSocket::WriteOnly);
  if (socket.waitForConnected(100)) {
    socket.write(toRawData(args));
    socket.flush();
    socket.disconnectFromServer();

    return false;
  }

  server_.removeServer(name_);
  server_.listen(name_);
  return true;
}

void SingleInstance::newConnection()
{
  auto client = new SingleInstanceClient(server_.nextPendingConnection());
  connect(client, &SingleInstanceClient::newInstance, this,
          &SingleInstance::newInstance);
}

QByteArray SingleInstance::toRawData(QStringList const &data) const
{
  QByteArray result;

  for (auto &item : data) {
    result.append(item.toUtf8());
    result.append('\0');
  }

  return result;
}
