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
#include "core/components/controls/controlmode.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace ControlMode {

class ControlModeTestAdapter : public ::ControlMode
{
 public:
  using ControlMode::ControlMode;

  using ControlMode::cleanControl;
  using ControlMode::exportControl;
  using ControlMode::importControl;
  using ControlMode::mode;
  using ControlMode::syncControl;
};

class ControlMock : public IControl
{
 public:
  MAKE_MOCK1(preInit, void(ICommandQueue &), override);
  MAKE_MOCK1(postInit, void(ICommandQueue &), override);
  MAKE_MOCK0(init, void(), override);
  MAKE_CONST_MOCK0(active, bool(), override);
  MAKE_MOCK1(activate, void(bool), override);
  MAKE_MOCK1(clean, void(ICommandQueue &), override);
  MAKE_MOCK1(sync, void(ICommandQueue &), override);
  MAKE_CONST_MOCK0(ID, std::string const &(), override);
  MAKE_MOCK1(importWith, void(Importable::Importer &), override);
  MAKE_CONST_MOCK1(exportWith, void(Exportable::Exporter &), override);
};

class ControlModeImporterStub : public ::ControlMode::Importer
{
 public:
  ControlModeImporterStub(std::string mode)
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

  std::string const &provideMode() const override
  {
    return mode_;
  }

 private:
  std::string mode_;
};

class ControlModeExporterMock : public ::ControlMode::Exporter
{
 public:
  MAKE_MOCK1(takeMode, void(std::string const &), override);
  MAKE_MOCK1(takeModes, void(std::vector<std::string> const &), override);
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("ControlMode tests", "[GPU][ControlMode]")
{
  std::string_view const id{"_id_"};
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("ID is assigned")
  {
    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    REQUIRE(ts.ID() == id);
  }

  SECTION("mode are set only for known control ids")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const controlMockID("_control_mock_");
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);

    ControlModeTestAdapter ts(id, std::move(controlMocks), true);

    ts.mode(controlMockID);
    REQUIRE(ts.mode() == controlMockID);

    ts.mode("unknown");
    REQUIRE(ts.mode() == controlMockID);
  }

  SECTION("Pre-init its controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    REQUIRE_CALL(*controlMock, preInit(trompeloeil::_));

    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    CommandQueueStub ctlCmds;
    ts.preInit(ctlCmds);
  }

  SECTION("Post-init its controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    REQUIRE_CALL(*controlMock, postInit(trompeloeil::_));

    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    CommandQueueStub ctlCmds;
    ts.postInit(ctlCmds);
  }

  SECTION("On initialization...")
  {
    SECTION("Initialize its controls")
    {
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      ControlMock *controlMock =
          static_cast<ControlMock *>(controlMocks[0].get());
      std::string const controlMockID("_control_mock_");
      REQUIRE_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);
      ALLOW_CALL(*controlMock, active()).RETURN(true);

      ControlModeTestAdapter ts(id, std::move(controlMocks), true);
      ts.init();
    }

    SECTION("Sets as active control the first active control, deactivating the "
            "following active controls")
    {
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      controlMocks.emplace_back(std::make_unique<ControlMock>());

      ControlMock *controlMock =
          static_cast<ControlMock *>(controlMocks[0].get());
      std::string const inactiveControlMockID("_inactive_control_mock_");
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(inactiveControlMockID);
      ALLOW_CALL(*controlMock, active()).RETURN(false);

      std::string const activeControlMockID("_active_control_mock_");

      controlMock = static_cast<ControlMock *>(controlMocks[1].get());
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(activeControlMockID);
      ALLOW_CALL(*controlMock, active()).RETURN(true);

      controlMock = static_cast<ControlMock *>(controlMocks[2].get());
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(activeControlMockID);
      ALLOW_CALL(*controlMock, active()).RETURN(true);
      REQUIRE_CALL(*controlMock, activate(false));

      ControlModeTestAdapter ts(id, std::move(controlMocks), true);
      ts.init();

      REQUIRE(ts.mode() == activeControlMockID);
    }

    SECTION("Set the first control as active when all controls are inactive")
    {
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      controlMocks.emplace_back(std::make_unique<ControlMock>());

      ControlMock *controlMock =
          static_cast<ControlMock *>(controlMocks[0].get());
      std::string const inactiveControlMockID0("_inactive_control_mock_0");
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(inactiveControlMockID0);
      ALLOW_CALL(*controlMock, active()).RETURN(false);
      REQUIRE_CALL(*controlMock, activate(true));

      controlMock = static_cast<ControlMock *>(controlMocks[1].get());
      std::string const inactiveControlMockID1("_inactive_control_mock_1");
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(inactiveControlMockID1);
      ALLOW_CALL(*controlMock, active()).RETURN(false);

      ControlModeTestAdapter ts(id, std::move(controlMocks), true);
      ts.init();

      REQUIRE(ts.mode() == inactiveControlMockID0);
    }
  }

  SECTION("Imports controls and mode")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());
    controlMocks.emplace_back(std::make_unique<ControlMock>());

    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const activeControlMockID("_active_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(activeControlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(true);
    REQUIRE_CALL(*controlMock, importWith(trompeloeil::_));
    REQUIRE_CALL(*controlMock, activate(false));

    controlMock = static_cast<ControlMock *>(controlMocks[1].get());
    std::string const newActiveControlMockID("_new_active_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(newActiveControlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(false);
    REQUIRE_CALL(*controlMock, importWith(trompeloeil::_));
    REQUIRE_CALL(*controlMock, activate(true));

    ControlModeImporterStub i("_new_active_control_mock_");
    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    ts.init();
    auto oldMode = ts.mode();
    ts.importControl(i);

    REQUIRE(ts.mode() != oldMode);
    REQUIRE(ts.mode() == newActiveControlMockID);
  }

  SECTION("Export controls and mode")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());

    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const controlMockID("_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(true);
    REQUIRE_CALL(*controlMock, exportWith(trompeloeil::_));

    std::vector<std::string> const modes{controlMockID};
    ControlModeExporterMock e;
    REQUIRE_CALL(e, takeModes(trompeloeil::eq(modes)));
    REQUIRE_CALL(e, takeMode(trompeloeil::eq(controlMockID)));

    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    ts.init();
    ts.exportControl(e);
  }

  SECTION("Clean controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());

    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const controlMockID("_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(true);
    REQUIRE_CALL(*controlMock, clean(trompeloeil::_));

    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    ts.init();
    CommandQueueStub ctlCmds;
    ts.cleanControl(ctlCmds);
  }

  SECTION("Sync controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());

    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const controlMockID("_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(true);
    REQUIRE_CALL(*controlMock, sync(trompeloeil::_));

    ControlModeTestAdapter ts(id, std::move(controlMocks), true);
    ts.init();
    CommandQueueStub ctlCmds;
    ts.syncControl(ctlCmds);
  }
}
} // namespace ControlMode
} // namespace Tests
