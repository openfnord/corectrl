// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/info/swinfo.h"

namespace Tests {
namespace SWInfo {

class ProviderStub : public ISWInfo::IProvider
{
 public:
  std::vector<std::pair<std::string, std::string>> provideInfo() override
  {
    std::vector<std::pair<std::string, std::string>> info;
    info.emplace_back("info_key", "info");
    return info;
  }
};

TEST_CASE("SWInfo tests", "[Info][SWInfo]")
{
  ::SWInfo ts;

  SECTION("Software info is collected on initialization")
  {
    REQUIRE(ts.keys().empty());

    std::vector<std::unique_ptr<ISWInfo::IProvider>> providers;
    providers.emplace_back(std::make_unique<ProviderStub>());

    ts.initialize(providers);
    auto keys = ts.keys();

    REQUIRE(keys.size() == 1);
    REQUIRE(keys.front() == "info_key");
    REQUIRE(ts.info("info_key") == "info");
  }
}
} // namespace SWInfo
} // namespace Tests
