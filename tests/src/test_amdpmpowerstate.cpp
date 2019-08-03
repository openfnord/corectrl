//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/stringdatasourcestub.h"
#include "core/components/controls/amd/pm/powerstate/pmpowerstate.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMPowerState {

class PMPowerStateTestAdapter : public ::AMD::PMPowerState
{
 public:
  using ::AMD::PMPowerState::PMPowerState;

  using ::AMD::PMPowerState::cleanControl;
  using ::AMD::PMPowerState::exportControl;
  using ::AMD::PMPowerState::importControl;
  using ::AMD::PMPowerState::mode;
  using ::AMD::PMPowerState::modes;
  using ::AMD::PMPowerState::syncControl;
};

class PMPowerStateImporterStub : public ::AMD::PMPowerState::Importer
{
 public:
  PMPowerStateImporterStub(std::string_view mode)
  : mode_(mode)
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

  std::string const &providePMPowerStateMode() const override
  {
    return mode_;
  }

 private:
  std::string mode_;
};

class PMPowerStateExporterMock : public ::AMD::PMPowerState::Exporter
{
 public:
  MAKE_MOCK1(takePMPowerStateMode, void(std::string const &), override);
  MAKE_MOCK1(takePMPowerStateModes, void(std::vector<std::string> const &),
             override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMPowerState tests", "[GPU][AMD][PM][PMPowerState]")
{
  std::string const powerDpmStatePath{"power_dpm_state"};
  CommandQueueStub ctlCmds;

  SECTION("Has PMPowerState ID")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.ID() == ::AMD::PMPowerState::ItemID);
  }

  SECTION("Is active by default")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.active());
  }

  SECTION("Has 'balanced' mode selected by default")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.mode() == "balanced");
  }

  SECTION("Has [battery, balanced, performance] modes")
  {
    std::vector<std::string> const modes{std::string("battery"),
                                         std::string("balanced"),
                                         std::string("performance")};
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    REQUIRE(ts.modes() == modes);
  }

  SECTION("mode only sets known modes")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    ts.mode("performance");
    REQUIRE(ts.mode() == "performance");

    ts.mode("unkown");
    REQUIRE(ts.mode() == "performance");
  }

  SECTION("Does not generate pre-init control commands")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    CommandQueueStub cmds;
    ts.preInit(cmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate post-init control commands")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());

    CommandQueueStub cmds;
    ts.postInit(cmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Imports its mode")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());
    PMPowerStateImporterStub i("performance");
    ts.importControl(i);

    REQUIRE(ts.mode() == "performance");
  }

  SECTION("Export its mode and available modes")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());
    PMPowerStateExporterMock e;

    trompeloeil::sequence seq;
    REQUIRE_CALL(e, takePMPowerStateModes(trompeloeil::_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMPowerStateMode(trompeloeil::eq("balanced")))
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands")
  {
    PMPowerStateTestAdapter ts(std::make_unique<StringDataSourceStub>());
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMPowerStateTestAdapter ts(
        std::make_unique<StringDataSourceStub>(powerDpmStatePath, "balanced"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when is out of sync")
  {
    PMPowerStateTestAdapter ts(
        std::make_unique<StringDataSourceStub>(powerDpmStatePath, "_other_"));
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().size() == 1);

    auto &[path, value] = ctlCmds.commands().front();
    REQUIRE(path == powerDpmStatePath);
    REQUIRE(value == "balanced");
  }
}

} // namespace PMPowerState
} // namespace AMD
} // namespace Tests
