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

#include "common/hwidtranslatorstub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/info/common/cpuinfolscpu.h"

namespace Tests {
namespace CPUInfoLsCpu {

TEST_CASE("CPUInfoLsCpu tests", "[Info][CPUInfo][CPUInfoLsCpu]")
{
  SECTION("Returns empty info when there is no data")
  {
    std::vector<std::string> const infoData;

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    REQUIRE(output.empty());
  }

  SECTION("Provides architecture")
  {
    std::vector<std::string> const infoData{"Architecture:        z48"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::arch),
                               std::string("z48"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides operation mode")
  {
    std::vector<std::string> const infoData{"CPU op-mode(s):      48-bit"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::opMode),
                               std::string("48-bit"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides byte order")
  {
    std::vector<std::string> const infoData{
        "Byte Order:          Middle Endian"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::byteOrder),
                               std::string("Middle Endian"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides virtualization")
  {
    std::vector<std::string> const infoData{"Virtualization:      zzz"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::virt),
                               std::string("zzz"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides L1d cache size")
  {
    std::vector<std::string> const infoData{"L1d cache:           320K"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::l1dCache),
                               std::string("320K"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides L1i cache size")
  {
    std::vector<std::string> const infoData{"L1i cache:           320K"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::l1iCache),
                               std::string("320K"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides L2 cache size")
  {
    std::vector<std::string> const infoData{"L2 cache:            640K"};

    ::CPUInfoLsCpu ts(
        std::make_unique<VectorStringDataSourceStub>("lscpu", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::CPUInfoLsCpu::Keys::l2Cache),
                               std::string("640K"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }
}
} // namespace CPUInfoLsCpu
} // namespace Tests
