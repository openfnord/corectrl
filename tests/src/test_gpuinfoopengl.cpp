// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/hwidtranslatorstub.h"
#include "core/idatasource.h"
#include "core/info/common/gpuinfoopengl.h"

namespace Tests {
namespace GPUInfoOpenGL {

class GPUInfoOpenGLDataSourceStub : public IDataSource<std::string, int const>
{
 public:
  GPUInfoOpenGLDataSourceStub(std::string source = "", std::string data = "")
  : source_(std::move(source))
  , data_(std::move(data))
  {
  }

  std::string source() const override
  {
    return source_;
  }

  bool read(std::string &data, int const &) override
  {
    data = data_;
    return true;
  }

 private:
  std::string const source_;
  std::string const data_;
};

TEST_CASE("GPUInfoOpenGL tests", "[Info][GPUInfo][GPUInfoOpenGL]")
{
  std::string const infoData(
      // clang-format off
"...\n\
Extended renderer info (GLX_MESA_query_renderer):\n\
...\n\
    Max core profile version: 1.1\n\
    Max compat profile version: 2.2\n\
..."); // clang-format on

  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorStub hwIDTranslator;

  ::GPUInfoOpenGL ts(
      std::make_unique<GPUInfoOpenGLDataSourceStub>("glxinfo", infoData));

  auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);

  SECTION("Provides core version")
  {
    auto coreVersion = std::make_pair(
        std::string(::GPUInfoOpenGL::Keys::coreVersion), std::string("1.1"));
    REQUIRE_THAT(output, Catch::VectorContains(coreVersion));
  }

  SECTION("Provides compat version")
  {
    auto compatVersion = std::make_pair(
        std::string(::GPUInfoOpenGL::Keys::compatVersion), std::string("2.2"));
    REQUIRE_THAT(output, Catch::VectorContains(compatVersion));
  }
}
} // namespace GPUInfoOpenGL
} // namespace Tests
