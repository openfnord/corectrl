// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
  using ::CommandQueue::packIndex;
  using ::CommandQueue::packWritesTo;
  using ::CommandQueue::toRawData;
};

TEST_CASE("CommandQueue tests", "[CommandQueue]")
{
  CommandQueueTestAdapter ts;

  SECTION("Initially, is empty")
  {
    REQUIRE(ts.commands().size() == 0);

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

      SECTION("Command packing mode...")
      {
        SECTION("Only has effect the first time it's activated")
        {
          REQUIRE_FALSE(ts.packIndex().has_value());

          ts.pack(true);

          REQUIRE(ts.packIndex().has_value());
          REQUIRE(*ts.packIndex() == 1);

          ts.add({"path1", "value1"});

          ts.pack(true);

          REQUIRE(ts.packIndex().has_value());
          REQUIRE(*ts.packIndex() == 1);
        }

        SECTION("Packs commands by file path")
        {
          ts.pack(true);
          ts.add({"other_path", "other_value"});
          ts.add({"path", "value1"});
          ts.add({"other_path", "other_value1"});

          auto commands = ts.commands();
          REQUIRE(commands.size() == 4);

          auto [path0, value0] = commands.at(0);
          REQUIRE(path0 == "path");
          REQUIRE(value0 == "value");

          auto [path1, value1] = commands.at(1);
          REQUIRE(path1 == "other_path");
          REQUIRE(value1 == "other_value");

          auto [path2, value2] = commands.at(2);
          REQUIRE(path2 == "other_path");
          REQUIRE(value2 == "other_value1");

          auto [path3, value3] = commands.at(3);
          REQUIRE(path3 == "path");
          REQUIRE(value3 == "value1");

          SECTION("Restores command queue behavior when it's deactivated")
          {
            ts.pack(false);
            ts.add({"other_path", "other_value2"});

            std::pair<std::string, std::string> last{"other_path",
                                                     "other_value2"};
            REQUIRE(ts.commands().back() == last);
          }
        }
      }

      SECTION("Check if queued commands writes to a file...")
      {
        SECTION("Returns nullopt when pack mode is not active")
        {
          REQUIRE(ts.packWritesTo("path") == std::nullopt);
        }

        SECTION("Returns false when there is no queued command in the pack")
        {
          ts.pack(true);
          ts.add({"other_path", "other_value"});

          auto res = ts.packWritesTo("path");
          REQUIRE(res.has_value());
          REQUIRE_FALSE(*res);
        }

        SECTION("Returns true when there is a queued command in the pack")
        {
          ts.pack(true);
          ts.add({"other_path", "other_value"});

          auto res = ts.packWritesTo("other_path");
          REQUIRE(res.has_value());
          REQUIRE(*res);
        }
      }

      SECTION("Commands are transformed into raw data...")
      {
        auto data = ts.toRawData();

        REQUIRE(data == QString("path\0value\0"));

        SECTION("The queue is cleared")
        {
          REQUIRE(ts.commands().size() == 0);
        }

        SECTION("Pack commands mode is deactivated")
        {
          ts.pack(true);
          auto _ = ts.toRawData();

          REQUIRE_FALSE(ts.packIndex().has_value());
        }
      }
    }
  }
}
} // namespace CommandQueue
} // namespace Tests
