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
