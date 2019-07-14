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
#include "catch.hpp"

#include "core/commandqueue.h"
#include <QString>

namespace Tests {
namespace CommandQueue {

class CommandQueueTestAdapter : public ::CommandQueue
{
 public:
  using ::CommandQueue::CommandQueue;

  using ::CommandQueue::add;
  using ::CommandQueue::commands;
  using ::CommandQueue::pack;
};

TEST_CASE("CommandQueue tests", "[CommandQueue]")
{
  CommandQueueTestAdapter ts;

  SECTION("Initially, is empty")
  {
    REQUIRE(ts.commands().empty());

    SECTION("Commands can be added")
    {
      ts.add({"path", "value"});

      REQUIRE(ts.commands().size() == 1);

      std::pair<std::string, std::string> cmd{"path", "value"};
      REQUIRE(ts.commands().front() == cmd);

      SECTION("Adding a command already queued has no effect")
      {
        ts.add({"path", "value"});

        REQUIRE(ts.commands().size() == 1);
      }

      SECTION("When commands are packed the queue is cleared")
      {
        QByteArray data;
        ts.pack(data);

        REQUIRE(ts.commands().empty());
        REQUIRE(data == QString("path\0value\0"));
      }
    }
  }
}
} // namespace CommandQueue
} // namespace Tests
