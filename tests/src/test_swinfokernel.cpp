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

#include "common/stringdatasourcestub.h"
#include "core/info/common/swinfokernel.h"

namespace Tests {
namespace SWInfoKernel {

TEST_CASE("SWInfoKernel tests", "[Info][SWInfo][SWInfoKernel]")
{
  SECTION("Provides kernel version")
  {
    std::string const infoData("Linux version 1.2.3_other_info ...");

    ::SWInfoKernel ts(
        std::make_unique<StringDataSourceStub>("/proc/version", infoData));
    auto output = ts.provideInfo();

    auto kernelVersion = std::make_pair(
        std::string(ISWInfo::Keys::kernelVersion), std::string("1.2.3"));
    REQUIRE_THAT(output, Catch::VectorContains(kernelVersion));
  }

  SECTION("Provides fake kernel version (0.0.0) when...")
  {
    SECTION("Cannot read /proc/version")
    {
      ::SWInfoKernel ts(
          std::make_unique<StringDataSourceStub>("/proc/version", "", false));
      auto output = ts.provideInfo();

      auto kernelVersion = std::make_pair(
          std::string(ISWInfo::Keys::kernelVersion), std::string("0.0.0"));
      REQUIRE_THAT(output, Catch::VectorContains(kernelVersion));
    }

    SECTION("/proc/version data changed its format")
    {
      std::string const infoData("Other format ...");

      ::SWInfoKernel ts(
          std::make_unique<StringDataSourceStub>("/proc/version", infoData));
      auto output = ts.provideInfo();

      auto kernelVersion = std::make_pair(
          std::string(ISWInfo::Keys::kernelVersion), std::string("0.0.0"));
      REQUIRE_THAT(output, Catch::VectorContains(kernelVersion));
    }
  }
}
} // namespace SWInfoKernel
} // namespace Tests
