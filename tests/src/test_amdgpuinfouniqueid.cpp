// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2022 Juan Palacios <jpalaciosdev@gmail.com>

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
