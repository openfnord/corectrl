// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/hwidtranslatorstub.h"
#include "common/stringpathdatasourcestub.h"
#include "core/info/amd/gpuinfovbios.h"

namespace Tests {
namespace AMD {
namespace GPUInfoVbios {

TEST_CASE("GPUInfoVbios tests", "[AMD][Info][GPUInfo][GPUInfoVbios]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorStub hwIDTranslator;

  SECTION("Provides the type given by its DataSource")
  {
    ::AMD::GPUInfoVbios ts(
        std::make_unique<StringPathDataSourceStub>("vbios_version", "VERSION"));
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

    auto type = std::make_pair(std::string(::AMD::GPUInfoVbios::version),
                               std::string("VERSION"));
    REQUIRE_THAT(output, Catch::VectorContains(type));
  }
}
} // namespace GPUInfoVbios
} // namespace AMD
} // namespace Tests
