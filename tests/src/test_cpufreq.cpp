// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/stringdatasourcestub.h"
#include "core/components/controls/cpu/cpufreq.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace CPUFreq {

class CPUFreqTestAdapter : public ::CPUFreq
{
 public:
  using ::CPUFreq::CPUFreq;

  using ::CPUFreq::cleanControl;
  using ::CPUFreq::exportControl;
  using ::CPUFreq::importControl;
  using ::CPUFreq::scalingGovernor;
  using ::CPUFreq::scalingGovernors;
  using ::CPUFreq::syncControl;
};

class CPUFreqImporterStub : public ::CPUFreq::Importer
{
 public:
  CPUFreqImporterStub(std::string_view scalingGovernor)
  : scalingGovernor_(scalingGovernor)
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

  std::string const &provideCPUFreqScalingGovernor() const override
  {
    return scalingGovernor_;
  }

 private:
  std::string scalingGovernor_;
};

class CPUFreqExporterMock : public ::CPUFreq::Exporter
{
 public:
  MAKE_MOCK1(takeCPUFreqScalingGovernor, void(std::string const &), override);
  MAKE_MOCK1(takeCPUFreqScalingGovernors,
             void(std::vector<std::string> const &), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD CPUFreq tests", "[CPU][CPUFreq]")
{
  std::vector<std::string> availableGovernors{"performance", "powersave"};
  std::vector<std::unique_ptr<IDataSource<std::string>>> scalingGovernorDataSources;
  std::string defaultGovernor{"powersave"};
  std::string const scalingGovernorPath{"scaling_governor"};
  CommandQueueStub ctlCmds;

  SECTION("Has CPUFreq ID")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    REQUIRE(ts.ID() == ::CPUFreq::ItemID);
  }

  SECTION("Is active by default")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    REQUIRE(ts.active());
  }

  SECTION("Has defaultGovernor selected by default when is available")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    REQUIRE(ts.scalingGovernor() == defaultGovernor);
  }

  SECTION("Has the first available scaling governor selected by default when "
          "defaultGovernor is not available")
  {
    std::vector<std::string> otherGovernors{"_other_0_", "_other_1_"};
    CPUFreqTestAdapter ts(std::move(otherGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    REQUIRE(ts.scalingGovernor() == "_other_0_");
  }

  SECTION("scalingGovernor only sets known scaling governors")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    ts.scalingGovernor("performance");
    REQUIRE(ts.scalingGovernor() == "performance");

    ts.scalingGovernor("unkown");
    REQUIRE(ts.scalingGovernor() == "performance");
  }

  SECTION("Does not generate pre-init control commands")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    CommandQueueStub cmds;
    ts.preInit(cmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    CommandQueueStub cmds;
    ts.postInit(cmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Imports its state")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));
    CPUFreqImporterStub i("performance");
    ts.importControl(i);

    REQUIRE(ts.scalingGovernor() == "performance");
  }

  SECTION("Export its state and available scaling governors")
  {
    auto governors = availableGovernors;
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));
    trompeloeil::sequence seq;
    CPUFreqExporterMock e;
    REQUIRE_CALL(e, takeCPUFreqScalingGovernors(trompeloeil::_))
        .LR_WITH(_1 == governors)
        .IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takeCPUFreqScalingGovernor(trompeloeil::eq(defaultGovernor)))
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    scalingGovernorDataSources.emplace_back(
        std::make_unique<StringDataSourceStub>(scalingGovernorPath,
                                               defaultGovernor));
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    scalingGovernorDataSources.emplace_back(
        std::make_unique<StringDataSourceStub>(scalingGovernorPath, "_other_"));
    CPUFreqTestAdapter ts(std::move(availableGovernors), defaultGovernor,
                          std::move(scalingGovernorDataSources));

    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);

    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == scalingGovernorPath);
    REQUIRE(value == defaultGovernor);
  }
}

} // namespace CPUFreq
} // namespace Tests
