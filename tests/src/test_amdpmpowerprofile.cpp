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
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/powerprofile/pmpowerprofile.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace AMD {
namespace PMPowerProfile {

class PMPowerProfileTestAdapter : public ::AMD::PMPowerProfile
{
 public:
  using ::AMD::PMPowerProfile::PMPowerProfile;

  using PMPowerProfile::cleanControl;
  using PMPowerProfile::exportControl;
  using PMPowerProfile::importControl;
  using PMPowerProfile::mode;
  using PMPowerProfile::modes;
  using PMPowerProfile::syncControl;
};

class PMPowerProfileImporterStub : public ::AMD::PMPowerProfile::Importer
{
 public:
  PMPowerProfileImporterStub(std::string mode)
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

  std::string const &providePMPowerProfileMode() const override
  {
    return mode_;
  }

 private:
  std::string mode_;
};

class PMPowerProfileExporterMock : public ::AMD::PMPowerProfile::Exporter
{
 public:
  MAKE_MOCK1(takePMPowerProfileModes, void(std::vector<std::string> const &),
             override);
  MAKE_MOCK1(takePMPowerProfileMode, void(std::string const &), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("AMD PMPowerProfile tests",
          "[GPU][AMD][PM][PMAdvanced][PMPowerProfile]")
{
  CommandQueueStub ctlCmds;
  std::vector<std::string> ppPowerProfileModeData{
      "PROFILE_INDEX(NAME) ", "  0 3D_FULL_SCREEN :", "  1   POWER_SAVING*:"};

  SECTION("Has PMPowerProfile ID")
  {
    PMPowerProfileTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                 std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.ID() == ::AMD::PMPowerProfile::ItemID);
  }

  SECTION("Is active by default")
  {
    PMPowerProfileTestAdapter ts(std::make_unique<StringDataSourceStub>(),
                                 std::make_unique<VectorStringDataSourceStub>());
    REQUIRE(ts.active());
  }

  SECTION("Initializes modes from data source on construction")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(),
        std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                     ppPowerProfileModeData));
    auto modes = ts.modes();
    REQUIRE(modes.size() == 2);

    REQUIRE(modes.front() == "3D_FULL_SCREEN");
    REQUIRE(modes.back() == "POWER_SAVING");

    SECTION("Has first mode selected by default")
    {
      REQUIRE(ts.mode() == "3D_FULL_SCREEN");
    }

    SECTION("Ignores unknown modes")
    {
      ts.mode("UNKNOWN");
      REQUIRE(ts.mode() == "3D_FULL_SCREEN");
    }
  }

  SECTION("Generate pre-init control commands")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "manual"),
        std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                     ppPowerProfileModeData));
    ts.preInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 2);

    auto &[cmd0Path, cmd0Value] = commands[0];
    REQUIRE(cmd0Path == "power_dpm_force_performance_level");
    REQUIRE(cmd0Value == "manual");

    auto &[cmd1Path, cmd1Value] = commands[1];
    REQUIRE(cmd1Path == "pp_power_profile_mode");
    REQUIRE(cmd1Value == "0");
  }

  SECTION("Import its mode")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(),
        std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                     ppPowerProfileModeData));
    ts.init();
    PMPowerProfileImporterStub i("POWER_SAVING");
    ts.importControl(i);

    REQUIRE(ts.mode() == "POWER_SAVING");
  }

  SECTION("Export its mode and available modes")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(),
        std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                     ppPowerProfileModeData));
    ts.init();

    trompeloeil::sequence seq;
    PMPowerProfileExporterMock e;
    REQUIRE_CALL(e, takePMPowerProfileModes(trompeloeil::_)).IN_SEQUENCE(seq);
    REQUIRE_CALL(e, takePMPowerProfileMode(trompeloeil::eq("3D_FULL_SCREEN")))
        .IN_SEQUENCE(seq);

    ts.exportControl(e);
  }

  SECTION("Does not generate clean control commands when there is no power "
          "profiles")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "manual"),
        std::make_unique<VectorStringDataSourceStub>());
    ts.init();
    ts.cleanControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Generate clean control commands when there is power profiles")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "manual"),
        std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                     ppPowerProfileModeData));
    ts.init();
    ts.cleanControl(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 2);

    auto &[cmd0Path, cmd0Value] = commands[0];
    REQUIRE(cmd0Path == "power_dpm_force_performance_level");
    REQUIRE(cmd0Value == "manual");

    auto &[cmd1Path, cmd1Value] = commands[1];
    REQUIRE(cmd1Path == "pp_power_profile_mode");
    REQUIRE(cmd1Value == "0");
  }

  SECTION("Does not generate sync control commands when there is no power "
          "profiles")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "manual"),
        std::make_unique<VectorStringDataSourceStub>());
    ts.init();
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does not generate sync control commands when is synced")
  {
    PMPowerProfileTestAdapter ts(
        std::make_unique<StringDataSourceStub>(
            "power_dpm_force_performance_level", "manual"),
        std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                     ppPowerProfileModeData));
    ts.init();
    ts.mode("POWER_SAVING");
    ts.syncControl(ctlCmds);

    REQUIRE(ctlCmds.commands().empty());
  }

  SECTION("Does generate sync control commands when...")
  {
    SECTION("power_dpm_force_performance_level is out of sync")
    {
      PMPowerProfileTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "_not_manual_"),
          std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                       ppPowerProfileModeData));
      ts.init();
      ts.mode("POWER_SAVING");
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 2);

      auto &[cmd0Path, cmd0Value] = commands[0];
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands[1];
      REQUIRE(cmd1Path == "pp_power_profile_mode");
      REQUIRE(cmd1Value == "1");
    }

    SECTION("pp_power_profile_mode is out of sync")
    {
      PMPowerProfileTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_power_profile_mode",
                                                       ppPowerProfileModeData));
      ts.init();
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().size() == 1);

      auto &[cmdPath, cmdValue] = ctlCmds.commands().front();
      REQUIRE(cmdPath == "pp_power_profile_mode");
      REQUIRE(cmdValue == "0");
    }
  }
}

} // namespace PMPowerProfile
} // namespace AMD
} // namespace Tests
