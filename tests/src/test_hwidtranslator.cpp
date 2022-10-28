// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "core/idatasource.h"
#include "core/info/hwidtranslator.h"

namespace Tests {
namespace HWIDTranslator {

class HWIDDataSourceStub : public IDataSource<std::vector<char>>
{
  std::string source() const override
  {
    return "";
  }

  bool read(std::vector<char> &data) override
  {
    data.clear();
    std::copy(data_.cbegin(), data_.cend(), std::back_inserter(data));
    return true;
  }

  // In pci.ids format (https://pci-ids.ucw.cz)
  static constexpr std::string_view data_{
      // clang-format off
"\
#	commentary\n\
# commentary\n\
\
1002 Vendor Name # (something)\n\
	aaaa  Device Name # [something]\n\
		bbbb  cccc Subdevice Name (something) [something] more text #\n\
\n\
C 11 Class Name # (something) [something]\n\
	aa  Subclass Name # (something) [something]\n\
		bb  Programming Interface (something) mode text #\n\
"
      // clang-format on
  };
};

TEST_CASE("HWIDTranslator tests", "[Info][HWIDTranslator]")
{
  std::vector<Vendor> vendors{Vendor::AMD};
  ::HWIDTranslator ts(vendors, std::make_unique<HWIDDataSourceStub>());

  SECTION("Parses pci-ids formatted data...")
  {
    SECTION("Extract vendors")
    {
      REQUIRE(ts.vendor("1002") == "Vendor Name #");
    }

    SECTION("Extract devices")
    {
      REQUIRE(ts.device("1002", "aaaa") == "Device Name #");
    }

    SECTION("Extract subdevices")
    {
      REQUIRE(ts.subdevice("1002", "aaaa", "bbbb", "cccc") == "Subdevice Name");
    }
  }
}
} // namespace HWIDTranslator
} // namespace Tests
