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

#include "core/info/cpuinfo.h"

namespace Tests {
namespace CPUInfo {

class ProviderStub : public ICPUInfo::IProvider
{
 public:
  std::vector<std::pair<std::string, std::string>>
  provideInfo(int, std::vector<ICPUInfo::ExecutionUnit> const &) override
  {
    std::vector<std::pair<std::string, std::string>> info;
    info.emplace_back("info_key", "info");
    return info;
  }

  std::vector<std::string>
  provideCapabilities(int, std::vector<ICPUInfo::ExecutionUnit> const &) override
  {
    std::vector<std::string> cap;
    cap.emplace_back("capability");
    return cap;
  }
};

TEST_CASE("CPUInfo tests", "[Info][CPUInfo]")
{
  int socketId{0};
  ::CPUInfo ts(socketId, {{0, 0, "/proc/cpu0"}});

  SECTION("Has CPU socket id")
  {
    REQUIRE(ts.socketId() == socketId);
  }

  SECTION("Has execution units")
  {
    REQUIRE_FALSE(ts.executionUnits().empty());

    auto &unit = ts.executionUnits().front();
    REQUIRE(unit.cpuId == 0);
    REQUIRE(unit.coreId == 0);
    REQUIRE(unit.sysPath == "/proc/cpu0");
  }

  SECTION("CPU info and capabilities are collected on initialization")
  {
    REQUIRE(ts.keys().empty());

    std::vector<std::unique_ptr<ICPUInfo::IProvider>> providers;
    providers.emplace_back(std::make_unique<ProviderStub>());

    ts.initialize(providers);
    auto keys = ts.keys();

    REQUIRE(keys.size() == 1);
    REQUIRE(keys.front() == "info_key");
    REQUIRE(ts.info("info_key") == "info");

    REQUIRE(ts.hasCapability("capability"));
  }
}
} // namespace CPUInfo
} // namespace Tests
