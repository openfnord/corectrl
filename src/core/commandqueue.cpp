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
#include <iterator>
#include <optional>

CommandQueue::CommandQueue() noexcept
: packIndex_(std::nullopt)
{
  commands().reserve(50);
}

void CommandQueue::pack(bool activate)
{
  if (activate) {
    // do not overwrite a previous index
    if (!packIndex_.has_value())
      packIndex_ = commands_.size();
  }
  else {
    packIndex_ = std::nullopt;
  }
}

std::optional<bool> CommandQueue::packWritesTo(std::string const &file)
{
  if (packIndex_.has_value()) {
    // find the last queued command that touch the same file
    auto it = std::find_if(commands().crbegin(), commands().crend(),
                           [&](auto &v) { return v.first == file; });
    if (it != commands().crend()) {
      auto index = std::distance(commands().cbegin(), it.base()) - 1;
      return index >= *packIndex();
    }
    else {
      return false;
    }
  }

  return std::nullopt;
}

void CommandQueue::add(std::pair<std::string, std::string> &&cmd)
{
  // find the last queued command that touch the same file
  auto lastIt = std::find_if(commands().crbegin(), commands().crend(),
                             [&](auto &v) { return v.first == cmd.first; });

  if (lastIt != commands().crend() && lastIt->second == cmd.second)
    return; // command already queued

  // insert command at the end by default
  auto insertIt = commands().cend();

  // update insertIt when lastIt is in pack range
  if (lastIt != commands().crend() && packIndex().has_value()) {

    auto index = std::distance(commands().cbegin(), lastIt.base()) - 1;
    if (index >= *packIndex())
      insertIt = lastIt.base();
  }

  commands().emplace(insertIt, std::move(cmd));
}

QByteArray CommandQueue::toRawData()
{
  QByteArray data;
  for (auto &[path, value] : commands()) {
    data += path.c_str();
    data += '\0';
    data += value.c_str();
    data += '\0';
  }

  commands().clear();
  packIndex_ = std::nullopt;
  return data;
}

unsigned int CommandQueue::count()
{
  return commands().size();
}

std::vector<std::pair<std::string, std::string>> &CommandQueue::commands()
{
  return commands_;
}

std::optional<unsigned int> const &CommandQueue::packIndex() const
{
  return packIndex_;
}
