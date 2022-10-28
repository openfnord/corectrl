// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/stringdatasourcestub.h"
#include "core/info/common/swinfomesa.h"

namespace Tests {
namespace SWInfoMesa {

TEST_CASE("SWInfoMesa tests", "[Info][SWInfo][SWInfoMesa]")
{
  std::string const infoData(
      // clang-format off
"...\n\
Extended renderer info (GLX_MESA_query_renderer):\n\
...\n\
    Version: 1.2.3\n\
..."); // clang-format on

  ::SWInfoMesa ts(std::make_unique<StringDataSourceStub>("glxinfo", infoData));
  auto output = ts.provideInfo();

  SECTION("Provides mesa version")
  {
    auto mesaVersion = std::make_pair(std::string(ISWInfo::Keys::mesaVersion),
                                      std::string("1.2.3"));
    REQUIRE_THAT(output, Catch::VectorContains(mesaVersion));
  }
}
} // namespace SWInfoMesa
} // namespace Tests
