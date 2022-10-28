// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/hwidtranslatorstub.h"
#include "common/stringpathdatasourcestub.h"
#include "core/info/common/gpuinforevision.h"

namespace Tests {
namespace GPUInfoRevision {

TEST_CASE("GPUInfoRevision tests", "[Info][GPUInfo][GPUInfoRevision]")
{
  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorStub hwIDTranslator;

  SECTION("Provides revision")
  {
    std::string const infoData("0x123");

    ::GPUInfoRevision ts(
        std::make_unique<StringPathDataSourceStub>("revision", infoData));
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

    auto revision = std::make_pair(std::string(IGPUInfo::Keys::revision),
                                   std::string("123"));
    REQUIRE_THAT(output, Catch::VectorContains(revision));
  }
}
} // namespace GPUInfoRevision
} // namespace Tests
