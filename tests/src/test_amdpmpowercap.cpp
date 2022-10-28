// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "core/components/controls/amd/pm/advanced/powercap/pmpowercap.h"
#include "core/idatasource.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMPowerCap {

class ULongDataSourceStub : public IDataSource<unsigned long>
{
 public:
  ULongDataSourceStub(std::string_view source = "", unsigned long data = 0,
                      bool success = true) noexcept
  : source_(source)
  , data_(data)
  , success_(success)
  {
  }

  std::string source() const override
  {
    return source_;
  }

  bool read(unsigned long &data) override
  {
    data = data_;
    return success_;
  }

  void data(unsigned long data)
  {
    data_ = data;
  }

  std::string const source_;
  unsigned long data_;
  bool success_;
};

class PMPowerCapTestAdapter : public ::AMD::PMPowerCap
{
 public:
  using ::AMD::PMPowerCap::PMPowerCap;

  using ::AMD::PMPowerCap::cleanControl;
  using ::AMD::PMPowerCap::exportControl;
  using ::AMD::PMPowerCap::importControl;
  using ::AMD::PMPowerCap::max;
  using ::AMD::PMPowerCap::min;
  using ::AMD::PMPowerCap::syncControl;
  using ::AMD::PMPowerCap::value;
};

class PMPowerCapImporterStub : public ::AMD::PMPowerCap::Importer
{
 public:
  PMPowerCapImporterStub(units::power::watt_t value)
  : value_(value)
  {
  }

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return *this;
  }

  bool provideActive() const override
  {
    return false;
  }

  units::power::watt_t providePMPowerCapValue() const override
  {
    return value_;
  }

 private:
  units::power::watt_t value_;
};

class PMPowerCapExporterMock : public ::AMD::PMPowerCap::Exporter
{
 public:
  MAKE_MOCK1(takePMPowerCapValue, void(units::power::watt_t), override);
  MAKE_MOCK2(takePMPowerCapRange,
             void(units::power::watt_t, units::power::watt_t), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMPowerCap tests", "[GPU][AMD][PM][PMPowerCap]")
{
  CommandQueueStub ctlCmds;
  units::power::watt_t min(50);
  units::power::watt_t max(100);

  SECTION("Has PMPowerCap ID")
  {
    PMPowerCapTestAdapter ts(std::make_unique<ULongDataSourceStub>(), min, max);

    REQUIRE(ts.ID() == ::AMD::PMPowerCap::ItemID);
  }

  SECTION("Is active by default")
  {
    PMPowerCapTestAdapter ts(std::make_unique<ULongDataSourceStub>(), min, max);

    REQUIRE(ts.active());
  }

  SECTION("Has 1 watt as minimum range value when 0 watt is supplied as the "
          "minimum value at construction")
  { // NOTE 0 watt is reserved for reseting the value
    PMPowerCapTestAdapter ts(std::make_unique<ULongDataSourceStub>(),
                             units::power::watt_t(0), max);

    REQUIRE(ts.min() == units::power::watt_t(1));
  }

  SECTION("Clamp its value in the [min, max] range")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap", 50000000), min, max);

    ts.value(units::power::watt_t(0));
    REQUIRE(ts.value() == min);

    ts.value(units::power::watt_t(500));
    REQUIRE(ts.value() == max);
  }

  SECTION("Generate pre-init control commands")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap", 10000000), min, max);
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmdPath, cmdValue] = commands.at(0);
    REQUIRE(cmdPath == "power1_cap");
    REQUIRE(cmdValue == "0");

    SECTION("Generate post-init control commands")
    {
      ctlCmds.clear();
      ts.postInit(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 1);

      auto &[cmdPath, cmdValue] = commands.at(0);
      REQUIRE(cmdPath == "power1_cap");
      REQUIRE(cmdValue == "10000000"); // restore pre-init value
    }
  }

  SECTION("Initializes power cap value from power1_cap data source")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap", 50000000), min, max);
    ts.init();

    REQUIRE(ts.value() == units::power::watt_t(50));
  }

  SECTION("Imports its state")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap"), min, max);

    PMPowerCapImporterStub i(units::power::watt_t(80));
    ts.importWith(i);

    REQUIRE(ts.value() == units::power::watt_t(80));
  }

  SECTION("Export its state")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap"), min, max);

    units::power::watt_t value(80);
    ts.value(value);

    trompeloeil::sequence seq;
    PMPowerCapExporterMock e;
    REQUIRE_CALL(e, takePMPowerCapRange(trompeloeil::_, trompeloeil::_))
        .LR_WITH(_1 == min)
        .LR_WITH(_2 == max)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMPowerCapValue(trompeloeil::_))
        .LR_WITH(_1 == value)
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Generate clean control commands")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap"), min, max);

    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmdPath, cmdValue] = commands.at(0);
    REQUIRE(cmdPath == "power1_cap");
    REQUIRE(cmdValue == "0");
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap", 50000000), min, max);
    ts.init();

    ts.syncControl(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    PMPowerCapTestAdapter ts(
        std::make_unique<ULongDataSourceStub>("power1_cap", 50000000), min, max);
    ts.init();
    ts.value(units::power::watt_t(80));

    ts.sync(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 1);

    auto &[cmdPath, cmdValue] = commands.at(0);
    REQUIRE(cmdPath == "power1_cap");
    REQUIRE(cmdValue == "80000000");
  }
}
} // namespace PMPowerCap
} // namespace AMD
} // namespace Tests
