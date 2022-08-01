//
// Copyright 2022 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "common/stringpathdatasourcestub.h"
#include "core/info/amd/gpuinfouniqueid.h"

namespace Tests {
namespace AMD {
namespace GPUInfoUniqueID {

TEST_CASE("GPUInfoUniqueID tests", "[AMD][Info][GPUInfo][GPUInfoUniqueID]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorStub hwIDTranslator;

  SECTION("Provides the type given by its DataSource")
  {
    ::AMD::GPUInfoUniqueID ts(std::make_unique<StringPathDataSourceStub>(
        "unique_id", "some_unique_id"));
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

    auto type = std::make_pair(std::string(::IGPUInfo::Keys::uniqueID),
                               std::string("SOME_UNIQUE_ID"));
    REQUIRE_THAT(output, Catch::VectorContains(type));
  }
}
} // namespace GPUInfoUniqueID
} // namespace AMD
} // namespace Tests
