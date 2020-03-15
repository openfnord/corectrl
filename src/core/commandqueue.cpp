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
#include "commandqueue.h"

#include <QByteArray>
#include <algorithm>
#include <utility>

CommandQueue::CommandQueue() noexcept
{
  commands().reserve(50);
}

void CommandQueue::add(std::pair<std::string, std::string> &&cmd)
{
  // find the last queued command that touch the same file
  auto it = std::find_if(commands().crbegin(), commands().crend(),
                         [&](auto &v) { return v.first == cmd.first; });
  if (it != commands().crend() && it->second == cmd.second)
    return; // command alredy in queue

  commands().emplace_back(std::move(cmd));
}

void CommandQueue::pack(QByteArray &data)
{
  for (auto &[path, value] : commands()) {
    data += path.c_str();
    data += '\0';
    data += value.c_str();
    data += '\0';
  }
  commands().clear();
}

std::vector<std::pair<std::string, std::string>> &CommandQueue::commands()
{
  return commands_;
}
