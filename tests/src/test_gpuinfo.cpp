// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/info/gpuinfo.h"
#include "core/info/ihwidtranslator.h"

namespace Tests {
namespace GPUInfo {

class ProviderStub : public IGPUInfo::IProvider
{
 public:
  std::vector<std::pair<std::string, std::string>>
  provideInfo(Vendor, int, IGPUInfo::Path const &,
              IHWIDTranslator const &) override
  {
    std::vector<std::pair<std::string, std::string>> info;
    info.emplace_back("info_key", "info");
    return info;
  }

  std::vector<std::string> provideCapabilities(Vendor, int,
                                               IGPUInfo::Path const &) override
  {
    std::vector<std::string> cap;
    cap.emplace_back("capability");
    return cap;
  }
};

class HWIDTranslatorStub : public IHWIDTranslator
{
 public:
  std::string vendor(std::string const &) const override
  {
    return "";
  }

  std::string device(std::string const &, std::string const &) const override
  {
    return "";
  }

  std::string subdevice(std::string const &, std::string const &,
                        std::string const &, std::string const &) const override
  {
    return "";
  }
};

TEST_CASE("GPUInfo tests", "[Info][GPUInfo]")
{
  int gpuIndex{123};
  ::GPUInfo ts(Vendor::AMD, gpuIndex, IGPUInfo::Path("_sys_", "_dev_"));

  SECTION("Has vendor")
  {
    REQUIRE(ts.vendor() == Vendor::AMD);
  }

  SECTION("Has GPU index")
  {
    REQUIRE(ts.index() == gpuIndex);
  }

  SECTION("Has device paths")
  {
    REQUIRE(ts.path().sys == "_sys_");
    REQUIRE(ts.path().dev == "_dev_");
  }

  SECTION("GPU info and capabilities are collected on initialization")
  {
    REQUIRE(ts.keys().empty());

    HWIDTranslatorStub hwidTranStub;
    std::vector<std::unique_ptr<IGPUInfo::IProvider>> providers;
    providers.emplace_back(std::make_unique<ProviderStub>());

    ts.initialize(providers, hwidTranStub);
    auto keys = ts.keys();

    REQUIRE(keys.size() == 1);
    REQUIRE(keys.front() == "info_key");
    REQUIRE(ts.info("info_key") == "info");

    REQUIRE(ts.hasCapability("capability"));
  }
}
} // namespace GPUInfo
} // namespace Tests
