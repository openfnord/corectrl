// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/ppdpmhandlermock.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreq.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFixedFreq {

class PMFixedFreqTestAdapter : public ::AMD::PMFixedFreq
{
 public:
  using ::AMD::PMFixedFreq::PMFixedFreq;

  using PMFixedFreq::cleanControl;
  using PMFixedFreq::exportControl;
  using PMFixedFreq::importControl;
  using PMFixedFreq::syncControl;
};

class PMFixedFreqImporterStub : public ::AMD::PMFixedFreq::Importer
{
 public:
  PMFixedFreqImporterStub(unsigned int sclkIndex, unsigned int mclkIndex)
  : sclkIndex_(sclkIndex)
  , mclkIndex_(mclkIndex)
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

  unsigned int providePMFixedFreqSclkIndex() const override
  {
    return sclkIndex_;
  }

  unsigned int providePMFixedFreqMclkIndex() const override
  {
    return mclkIndex_;
  }

 private:
  unsigned int sclkIndex_;
  unsigned int mclkIndex_;
};

class PMFixedFreqExporterMock : public ::AMD::PMFixedFreq::Exporter
{
 public:
  MAKE_MOCK1(takePMFixedFreqSclkIndex, void(unsigned int), override);
  MAKE_MOCK1(takePMFixedFreqMclkIndex, void(unsigned int), override);
  MAKE_MOCK1(
      takePMFixedFreqSclkStates,
      void(std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &),
      override);
  MAKE_MOCK1(
      takePMFixedFreqMclkStates,
      void(std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &),
      override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFixedFreq tests", "[GPU][AMD][PM][PMAdvanced][PMFixedFreq]")
{
  CommandQueueStub ctlCmds;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states{
      {0, units::frequency::megahertz_t(300)},
      {1, units::frequency::megahertz_t(2000)}};
  auto ppDpmSclkMock = std::make_unique<PpDpmHandlerMock>(states);
  auto ppDpmMclkMock = std::make_unique<PpDpmHandlerMock>(states);

  std::vector<unsigned int> activeStates{0};

  SECTION("Has PMFixedFreq ID")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    REQUIRE(ts.ID() == ::AMD::PMFixedFreq::ItemID);
  }

  SECTION("Is active by default")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    REQUIRE(ts.active());
  }

  SECTION("Has first state selected by default")
  {
    REQUIRE_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    REQUIRE_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
  }

  SECTION("Does not generate pre-init control commands")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.preInit(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    ts.preInit(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Import its state")
  {
    REQUIRE_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    REQUIRE_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    auto &ppDpmSclkMockRef = *ppDpmSclkMock;
    auto &ppDpmMclkMockRef = *ppDpmMclkMock;

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));

    std::vector<unsigned int> sclkState{0};
    std::vector<unsigned int> mclkState{1};
    REQUIRE_CALL(ppDpmSclkMockRef, activate(trompeloeil::_)).WITH(_1 == sclkState);
    REQUIRE_CALL(ppDpmMclkMockRef, activate(trompeloeil::_)).WITH(_1 == mclkState);

    PMFixedFreqImporterStub i(0, 1);
    ts.importControl(i);
  }

  SECTION("Export its state and available states")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    REQUIRE_CALL(*ppDpmSclkMock, active()).LR_RETURN(activeStates);
    REQUIRE_CALL(*ppDpmMclkMock, active()).LR_RETURN(activeStates);

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));
    trompeloeil::sequence seq;
    PMFixedFreqExporterMock e;
    REQUIRE_CALL(e, takePMFixedFreqSclkStates(trompeloeil::_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFixedFreqSclkIndex(trompeloeil::eq(0u))).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFixedFreqMclkStates(trompeloeil::_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFixedFreqMclkIndex(trompeloeil::eq(0u))).IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Resets ppDpmHandlers on clean")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    REQUIRE_CALL(*ppDpmSclkMock, reset(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMclkMock, reset(trompeloeil::_));

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));

    ts.cleanControl(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Sync ppDpmHandlers on sync")
  {
    ALLOW_CALL(*ppDpmSclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    ALLOW_CALL(*ppDpmMclkMock, activate(trompeloeil::_)).WITH(_1 == activeStates);
    REQUIRE_CALL(*ppDpmSclkMock, sync(trompeloeil::_));
    REQUIRE_CALL(*ppDpmMclkMock, sync(trompeloeil::_));

    PMFixedFreqTestAdapter ts(std::move(ppDpmSclkMock), std::move(ppDpmMclkMock));

    ts.syncControl(ctlCmds);
    REQUIRE(ctlCmds.commands().empty());
  }
}

} // namespace PMFixedFreq
} // namespace AMD
} // namespace Tests
