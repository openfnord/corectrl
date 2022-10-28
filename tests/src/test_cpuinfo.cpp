// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
