// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"

#include "common/commandqueuestub.h"
#include "common/controlmock.h"
#include "common/stringdatasourcestub.h"
#include "common/vectorstringdatasourcestub.h"
#include "core/components/controls/amd/pm/advanced/overdrive/pmoverdrive.h"

namespace Tests {
namespace AMD {
namespace PMOverdrive {

class PMOverdriveTestAdapter : public ::AMD::PMOverdrive
{
 public:
  using ::AMD::PMOverdrive::PMOverdrive;

  using ::AMD::PMOverdrive::cleanControl;
  using ::AMD::PMOverdrive::perfLevelPreInitValue;
  using ::AMD::PMOverdrive::syncControl;
};

TEST_CASE("AMD PMOverdrive tests", "[GPU][AMD][PM][PMOverdrive]")
{
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("Has PMOverdrive ID")
  {
    ::AMD::PMOverdrive ts(std::make_unique<StringDataSourceStub>(),
                          std::make_unique<VectorStringDataSourceStub>(),
                          std::move(controlMocks));
    REQUIRE(ts.ID() == ::AMD::PMOverdrive::ItemID);
  }

  SECTION("Is not active by default")
  {
    ::AMD::PMOverdrive ts(std::make_unique<StringDataSourceStub>(),
                          std::make_unique<VectorStringDataSourceStub>(),
                          std::move(controlMocks));
    REQUIRE_FALSE(ts.active());
  }

  SECTION("Pre-init its controls, generating pre-init overdrive commands...")
  {
    std::vector<std::string> ppOdClkVoltageData{"..."};

    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    REQUIRE_CALL(*controlMock, preInit(trompeloeil::_));

    SECTION("Including performance level command when its value is not manual")
    {
      PMOverdriveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "auto"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(controlMocks));

      CommandQueueStub ctlCmds;
      ts.preInit(ctlCmds);

      auto perfLevelEntry = ts.perfLevelPreInitValue();
      REQUIRE(perfLevelEntry == "auto");

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 3);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "r");

      auto &[cmd2Path, cmd2Value] = commands.at(2);
      REQUIRE(cmd2Path == "pp_od_clk_voltage");
      REQUIRE(cmd2Value == "c");
    }

    SECTION("Excluding performance level command when its value is manual")
    {
      PMOverdriveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(controlMocks));

      CommandQueueStub ctlCmds;
      ts.preInit(ctlCmds);

      auto perfLevelEntry = ts.perfLevelPreInitValue();
      REQUIRE(perfLevelEntry == "manual");

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 2);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "pp_od_clk_voltage");
      REQUIRE(cmd0Value == "r");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "c");
    }
  }

  SECTION("Post-init its controls and generate post-init overdrive commands")
  {
    std::vector<std::string> ppOdClkVoltageData{"..."};

    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    ALLOW_CALL(*controlMock, preInit(trompeloeil::_));
    REQUIRE_CALL(*controlMock, postInit(trompeloeil::_));

    PMOverdriveTestAdapter ts(std::make_unique<StringDataSourceStub>(
                                  "power_dpm_force_performance_level", "auto"),
                              std::make_unique<VectorStringDataSourceStub>(
                                  "pp_od_clk_voltage", ppOdClkVoltageData),
                              std::move(controlMocks));

    CommandQueueStub ctlCmds;
    ts.preInit(ctlCmds);
    ctlCmds.clear();
    ts.postInit(ctlCmds);

    auto &commands = ctlCmds.commands();
    REQUIRE(commands.size() == 2);

    auto &[cmd0Path, cmd0Value] = commands.at(0);
    REQUIRE(cmd0Path == "pp_od_clk_voltage");
    REQUIRE(cmd0Value == "c");

    auto &[cmd1Path, cmd1Value] = commands.at(1);
    REQUIRE(cmd1Path == "power_dpm_force_performance_level");
    REQUIRE(cmd1Value == "auto");
  }

  SECTION("Generate overdrive clean controls commands...")
  {
    std::vector<std::string> ppOdClkVoltageData{"..."};

    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    REQUIRE_CALL(*controlMock, clean(trompeloeil::_));

    SECTION("Including performance level command when its value is not manual")
    {
      PMOverdriveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "auto"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(controlMocks));

      CommandQueueStub ctlCmds;
      ts.cleanControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 3);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "r");

      auto &[cmd2Path, cmd2Value] = commands.at(2);
      REQUIRE(cmd2Path == "pp_od_clk_voltage");
      REQUIRE(cmd2Value == "c");
    }

    SECTION("Excluding performance level command when its value is manual")
    {
      PMOverdriveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(controlMocks));

      CommandQueueStub ctlCmds;
      ts.cleanControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 2);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "pp_od_clk_voltage");
      REQUIRE(cmd0Value == "r");

      auto &[cmd1Path, cmd1Value] = commands.at(1);
      REQUIRE(cmd1Path == "pp_od_clk_voltage");
      REQUIRE(cmd1Value == "c");
    }
  }

  SECTION("Sync its controls, generating sync control commands when is out of "
          "sync...")
  {
    std::vector<std::string> ppOdClkVoltageData{"..."};

    SECTION("Including performance level command when its value is not manual")
    {
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      ControlMock *controlMock =
          static_cast<ControlMock *>(controlMocks[0].get());
      REQUIRE_CALL(*controlMock, sync(trompeloeil::_));

      PMOverdriveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "auto"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(controlMocks));

      CommandQueueStub ctlCmds;
      ts.syncControl(ctlCmds);

      auto &commands = ctlCmds.commands();
      REQUIRE(commands.size() == 1);

      auto &[cmd0Path, cmd0Value] = commands.at(0);
      REQUIRE(cmd0Path == "power_dpm_force_performance_level");
      REQUIRE(cmd0Value == "manual");
    }

    SECTION("Excluding performance level command when its value is manual")
    {
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      ControlMock *controlMock =
          static_cast<ControlMock *>(controlMocks[0].get());
      REQUIRE_CALL(*controlMock, sync(trompeloeil::_));

      PMOverdriveTestAdapter ts(
          std::make_unique<StringDataSourceStub>(
              "power_dpm_force_performance_level", "manual"),
          std::make_unique<VectorStringDataSourceStub>("pp_od_clk_voltage",
                                                       ppOdClkVoltageData),
          std::move(controlMocks));

      CommandQueueStub ctlCmds;
      ts.syncControl(ctlCmds);

      REQUIRE(ctlCmds.commands().empty());
    }
  }
}
} // namespace PMOverdrive
} // namespace AMD
} // namespace Tests
