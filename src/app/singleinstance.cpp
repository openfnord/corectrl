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
#include "singleinstance.h"

#include <QLocalSocket>

SingleInstance::SingleInstance(std::string_view name, QObject *parent)
: QObject(parent)
, name_(name.data())
{
  connect(&server_, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

bool SingleInstance::tryMakeMainInstance()
{
  QLocalSocket socket;
  socket.connectToServer(name_, QLocalSocket::ReadOnly);
  if (socket.waitForConnected(100))
    return false;

  server_.removeServer(name_);
  server_.listen(name_);
  return true;
}

void SingleInstance::newConnection()
{
  emit newInstance();
}
