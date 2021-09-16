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

#include "core/icommandqueue.h"
#include <QByteArray>
#include <vector>

class CommandQueueStub final : public ICommandQueue
{
 public:
  void add(std::pair<std::string, std::string> &&cmd) override
  {
    commands_.emplace_back(std::move(cmd));
  }

  QByteArray toRawData() override
  {
    return QByteArray();
  }

  unsigned int count() override
  {
    return commands_.size();
  }

  std::vector<std::pair<std::string, std::string>> const &commands()
  {
    return commands_;
  }

  void clear()
  {
    commands_.clear();
  }

 private:
  std::vector<std::pair<std::string, std::string>> commands_;
};
