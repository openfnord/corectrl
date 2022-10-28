// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "core/components/controls/amd/pm/fixed/pmfixed.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMFixed {

class PMFixedTestAdapter : public ::AMD::PMFixed
{
 public:
  using ::AMD::PMFixed::PMFixed;

  using ::AMD::PMFixed::exportControl;
  using ::AMD::PMFixed::importControl;
  using ::AMD::PMFixed::mode;

  MAKE_MOCK1(cleanControl, void(ICommandQueue &), override);
  MAKE_MOCK1(syncControl, void(ICommandQueue &), override);
  MAKE_CONST_MOCK0(modes, std::vector<std::string> const &(), override);
};

class PMFixedImporterStub : public ::AMD::PMFixed::Importer
{
 public:
  PMFixedImporterStub(std::string mode)
  : mode_(std::move(mode))
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

  std::string const &providePMFixedMode() const override
  {
    return mode_;
  }

 private:
  std::string mode_;
};

class PMFixedExporterMock : public ::AMD::PMFixed::Exporter
{
 public:
  MAKE_MOCK1(takePMFixedMode, void(std::string const &), override);
  MAKE_MOCK1(takePMFixedModes, void(std::vector<std::string> const &), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMFixed tests", "[GPU][AMD][PM][PMFixed]")
{
  std::vector<std::string> const modes{"_mode_0_", "_mode_1_"};
  PMFixedTestAdapter ts("_mode_0_");
  ALLOW_CALL(ts, modes()).LR_RETURN(modes);

  SECTION("Has PMFixed ID")
  {
    REQUIRE(ts.ID() == ::AMD::PMFixed::ItemID);
  }

  SECTION("Is not active by default")
  {
    REQUIRE_FALSE(ts.active());
  }

  SECTION("mode only sets known modes")
  {
    REQUIRE_CALL(ts, modes()).LR_RETURN(modes);

    ts.mode("_mode_1_");
    REQUIRE(ts.mode() == "_mode_1_");

    ts.mode("unkown");
    REQUIRE(ts.mode() == "_mode_1_");
  }

  SECTION("Does not generate pre-init control commands")
  {
    CommandQueueStub cmds;
    ts.preInit(cmds);
    REQUIRE(cmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    CommandQueueStub cmds;
    ts.postInit(cmds);
    REQUIRE(cmds.commands().empty());
  }

  SECTION("Imports its mode")
  {
    PMFixedImporterStub i("_mode_1_");
    ts.importControl(i);

    REQUIRE(ts.mode() == "_mode_1_");
  }

  SECTION("Export its mode and available modes")
  {
    trompeloeil::sequence seq;
    PMFixedExporterMock e;
    REQUIRE_CALL(e, takePMFixedModes(trompeloeil::_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMFixedMode(trompeloeil::eq("_mode_0_"))).IN_SEQUENCE(seq);

    ts.exportControl(e);
  }
}

} // namespace PMFixed
} // namespace AMD
} // namespace Tests
