// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/uintdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/overclock/freqod/pmfreqod.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFreqOd {

class PMFreqOdTestAdapter : public ::AMD::PMFreqOd
{
 public:
  using ::AMD::PMFreqOd::PMFreqOd;

  using PMFreqOd::baseMclk;
  using PMFreqOd::baseSclk;
  using PMFreqOd::cleanControl;
  using PMFreqOd::exportControl;
  using PMFreqOd::importControl;
  using PMFreqOd::mclkOd;
  using PMFreqOd::sclkOd;
  using PMFreqOd::syncControl;
};

class PMFreqOdImporterStub : public ::AMD::PMFreqOd::Importer
{
 public:
  PMFreqOdImporterStub(unsigned int sclkOd, unsigned int mclkOd)
  : sclkOd_(sclkOd)
  , mclkOd_(mclkOd)
  {
  }

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return {};
  }

  bool provideActive() const override
  {
    return false;
  }

  unsigned int providePMFreqOdSclkOd() const override
  {
    return sclkOd_;
  }

  unsigned int providePMFreqOdMclkOd() const override
  {
    return mclkOd_;
  }

 private:
  unsigned int sclkOd_;
  unsigned int mclkOd_;
};

class PMFreqOdExporterMock : public ::AMD::PMFreqOd::Exporter
{
 public:
  MAKE_MOCK1(takePMFreqOdSclkOd, void(unsigned int), override);
  MAKE_MOCK1(takePMFreqOdMclkOd, void(unsigned int), override);
  MAKE_MOCK1(takePMFreqOdBaseSclk, void(units::frequency::megahertz_t), override);
  MAKE_MOCK1(takePMFreqOdBaseMclk, void(units::frequency::megahertz_t), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFreqOd tests",
          "[GPU][AMD][PM][PMAdvanced][PMOverclock][PMFreqOd]")
{
  CommandQueueStub ctlCmds;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const states{
      std::make_pair(0u, units::frequency::megahertz_t(300)),
      std::make_pair(1u, units::frequency::megahertz_t(2000))};

  SECTION("Has PMFreqOd ID")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    REQUIRE(ts.ID() == ::AMD::PMFreqOd::ItemID);
  }

  SECTION("Is active by default")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    REQUIRE(ts.active());
  }

  SECTION("Has 0 as sclk & mclk od values by default")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    REQUIRE(ts.sclkOd() == 0);
    REQUIRE(ts.mclkOd() == 0);
  }

  SECTION("Initializes base sclk from sclk states on construction")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.init();
    REQUIRE(ts.baseSclk() == units::frequency::megahertz_t(2000));
  }

  SECTION("Initializes base mclk from mclk states on construction")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.init();
    REQUIRE(ts.baseMclk() == units::frequency::megahertz_t(2000));
  }

  SECTION("Clamps sclkOd in [0, 20] range")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.sclkOd(21);
    REQUIRE(ts.sclkOd() == 20);
  }

  SECTION("Clamps mclkOd in [0, 20] range")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.mclkOd(21);
    REQUIRE(ts.mclkOd() == 20);
  }

  SECTION("Does not generate pre-init control commands")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.preInit(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.postInit(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Import its state")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.init();
    PMFreqOdImporterStub i(2, 3);
    ts.importControl(i);

    REQUIRE(ts.sclkOd() == 2);
    REQUIRE(ts.mclkOd() == 3);
  }

  SECTION("Export its state and available states")
  {
    PMFreqOdTestAdapter ts(std::make_unique<UIntDataSourceStub>(),
                           std::make_unique<UIntDataSourceStub>(), states,
                           states);
    ts.init();

    trompeloeil::sequence seq;
    PMFreqOdExporterMock e;
    REQUIRE_CALL(e, takePMFreqOdBaseSclk(
                        trompeloeil::eq(units::frequency::megahertz_t(2000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqOdBaseMclk(
                        trompeloeil::eq(units::frequency::megahertz_t(2000))))
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqOdSclkOd(trompeloeil::eq(0u))).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFreqOdMclkOd(trompeloeil::eq(0u))).IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Generate clean control commands unconditionally")
  {
    PMFreqOdTestAdapter ts(
        std::make_unique<UIntDataSourceStub>("pp_sclk_od", 0),
        std::make_unique<UIntDataSourceStub>("pp_mclk_od", 0), states, states);
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 2);

    auto &[cmd0Path, cmd0Value] = commands.front();
    REQUIRE(cmd0Path == "pp_sclk_od");
    REQUIRE(cmd0Value == "0");

    auto &[cmd1Path, cmd1Value] = commands.back();
    REQUIRE(cmd1Path == "pp_mclk_od");
    REQUIRE(cmd1Value == "0");
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMFreqOdTestAdapter ts(
        std::make_unique<UIntDataSourceStub>("pp_sclk_od", 1),
        std::make_unique<UIntDataSourceStub>("pp_mclk_od", 2), states, states);
    ts.init();
    ts.sclkOd(1);
    ts.mclkOd(2);
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("pp_sclk_od is out of sync")
    {
      PMFreqOdTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pp_sclk_od", 0),
          std::make_unique<UIntDataSourceStub>("pp_mclk_od", 0), states, states);
      ts.init();
      ts.sclkOd(1);
      ts.mclkOd(0);
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);
      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pp_sclk_od");
      REQUIRE(cmdValue == "1");
    }

    SECTION("pp_mclk_od is out of sync")
    {
      PMFreqOdTestAdapter ts(
          std::make_unique<UIntDataSourceStub>("pp_sclk_od", 0),
          std::make_unique<UIntDataSourceStub>("pp_mclk_od", 0), states, states);
      ts.init();
      ts.sclkOd(0);
      ts.mclkOd(1);
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);
      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pp_mclk_od");
      REQUIRE(cmdValue == "1");
    }
  }
}

} // namespace PMFreqOd
} // namespace AMD
} // namespace Tests
