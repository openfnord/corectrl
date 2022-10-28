// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "core/idatasource.h"
#include "core/info/common/gpuinfouevent.h"
#include "core/info/ihwidtranslator.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

using trompeloeil::_;

namespace Tests {
namespace GPUInfoUevent {

class GPUInfoUeventDataSourceStub
: public IDataSource<std::vector<std::string>, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "uevent";
  }

  bool read(std::vector<std::string> &data, std::filesystem::path const &) override
  {
    data.clear();
    data.emplace_back("DRIVER=driver");
    data.emplace_back("PCI_ID=1111:2222");
    data.emplace_back("PCI_SUBSYS_ID=3333:4444");
    data.emplace_back("PCI_SLOT_NAME=0000:01:00.0");
    return true;
  }
};

class HWIDTranslatorMock : public IHWIDTranslator
{
 public:
  MAKE_CONST_MOCK1(vendor, std::string(std::string const &));
  MAKE_CONST_MOCK2(device, std::string(std::string const &, std::string const &));
  MAKE_CONST_MOCK4(subdevice,
                   std::string(std::string const &, std::string const &,
                               std::string const &, std::string const &));
};

TEST_CASE("GPUInfoUevent tests", "[Info][GPUInfo][GPUInfoUevent]")
{
  std::string vName("v_name");
  std::string dName("d_name");
  std::string sdName("s_name");

  Vendor vendor(Vendor::AMD);
  int const gpuIndex = 0;
  IGPUInfo::Path path("_sys_", "_dev_");
  HWIDTranslatorMock hwIDTranslator;
  ALLOW_CALL(hwIDTranslator, vendor(_)).LR_RETURN(vName);
  ALLOW_CALL(hwIDTranslator, device(_, _)).LR_RETURN(dName);
  ALLOW_CALL(hwIDTranslator, subdevice(_, _, _, _)).LR_RETURN(sdName);

  ::GPUInfoUevent ts(std::make_unique<GPUInfoUeventDataSourceStub>());

  SECTION("Provides driver")
  {
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto driver = std::make_pair(std::string(IGPUInfo::Keys::driver),
                                 std::string("driver"));

    REQUIRE_THAT(output, Catch::VectorContains(driver));
  }

  SECTION("Provides PCI slot")
  {
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto driver = std::make_pair(std::string(IGPUInfo::Keys::pciSlot),
                                 std::string("0000:01:00.0"));

    REQUIRE_THAT(output, Catch::VectorContains(driver));
  }

  SECTION("Provides vendor id")
  {
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto vendorID = std::make_pair(std::string(IGPUInfo::Keys::vendorID),
                                   std::string("1111"));

    REQUIRE_THAT(output, Catch::VectorContains(vendorID));
  }

  SECTION("Provides vendor name")
  {
    REQUIRE_CALL(hwIDTranslator, vendor("1111")).LR_RETURN(vName);
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto vendorName = std::make_pair(std::string(IGPUInfo::Keys::vendorName),
                                     vName);

    REQUIRE_THAT(output, Catch::VectorContains(vendorName));
  }

  SECTION("Provides device id")
  {
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto deviceID = std::make_pair(std::string(IGPUInfo::Keys::deviceID),
                                   std::string("2222"));

    REQUIRE_THAT(output, Catch::VectorContains(deviceID));
  }

  SECTION("Provides device name")
  {
    REQUIRE_CALL(hwIDTranslator, device("1111", "2222")).LR_RETURN(dName);

    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto deviceName = std::make_pair(std::string(IGPUInfo::Keys::deviceName),
                                     dName);

    REQUIRE_THAT(output, Catch::VectorContains(deviceName));
  }

  SECTION("Provides subvendor id")
  {
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto subvendorID = std::make_pair(std::string(IGPUInfo::Keys::subvendorID),
                                      std::string("3333"));

    REQUIRE_THAT(output, Catch::VectorContains(subvendorID));
  }

  SECTION("Provides subdevice id")
  {
    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto subdeviceID = std::make_pair(std::string(IGPUInfo::Keys::subdeviceID),
                                      std::string("4444"));

    REQUIRE_THAT(output, Catch::VectorContains(subdeviceID));
  }

  SECTION("Provides subdevice name")
  {
    REQUIRE_CALL(hwIDTranslator, subdevice("1111", "2222", "3333", "4444"))
        .LR_RETURN(sdName);

    auto output = ts.provideInfo(vendor, gpuIndex, path, hwIDTranslator);
    auto subdeviceName = std::make_pair(
        std::string(IGPUInfo::Keys::subdeviceName), sdName);

    REQUIRE_THAT(output, Catch::VectorContains(subdeviceName));
  }
}
} // namespace GPUInfoUevent
} // namespace Tests
