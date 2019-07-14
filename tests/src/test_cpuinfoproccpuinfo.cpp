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
#include "core/info/common/cpuinfoproccpuinfo.h"

namespace Tests {
namespace CPUInfoProcCpuInfo {

TEST_CASE("CPUInfoProcCpuInfo tests", "[Info][CPUInfo][CPUInfoProcCpuInfo]")
{
  std::vector<std::string> infoData{"processor	: 0"};

  SECTION("Returns empty info when there is no data")
  {
    infoData.clear();

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData, false));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    REQUIRE(output.empty());
  }

  SECTION("Provides execution units count")
  {
    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::executionUnits),
                               std::string("1"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides vendor id")
  {
    infoData.emplace_back("vendor_id	: TheVendor");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::vendorId),
                               std::string("TheVendor"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides cpu family")
  {
    infoData.emplace_back("cpu family	: 128");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::cpuFamily),
                               std::string("128"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides model")
  {
    infoData.emplace_back("model		: 8");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::model),
                               std::string("8"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides model name")
  {
    infoData.emplace_back("model name	: Potato");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::modelName),
                               std::string("Potato"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides stepping")
  {
    infoData.emplace_back("stepping	: 2");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::stepping),
                               std::string("2"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides microcode")
  {
    infoData.emplace_back("microcode	: 0x01");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::ucode),
                               std::string("0x01"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides L3 cache size")
  {
    infoData.emplace_back("cache size	: 8192 KB");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::l3Cache),
                               std::string("8192 KB"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides cores count")
  {
    infoData.emplace_back("cpu cores	: 64");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::cores),
                               std::string("64"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides flags")
  {
    infoData.emplace_back("flags		: a lot");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::flags),
                               std::string("a lot"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides bugs")
  {
    infoData.emplace_back("bugs		: yes");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::bugs),
                               std::string("yes"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }

  SECTION("Provides bogomips")
  {
    infoData.emplace_back("bogomips	: 20.45");

    ::CPUInfoProcCpuInfo ts(std::make_unique<VectorStringDataSourceStub>(
        "/proc/cpuinfo", infoData));

    auto output = ts.provideInfo(0, {{0, 0, "/cpu0"}});
    auto data = std::make_pair(std::string(::ICPUInfo::Keys::bogomips),
                               std::string("20.45"));
    REQUIRE_THAT(output, Catch::VectorContains(data));
  }
}
} // namespace CPUInfoProcCpuInfo
} // namespace Tests
