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
#include "core/components/controls/controlgroup.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace ControlGroup {

class ControlGroupTestAdapter : public ::ControlGroup
{
 public:
  using ControlGroup::ControlGroup;

  using ControlGroup::cleanControl;
  using ControlGroup::exportControl;
  using ControlGroup::importControl;
  using ControlGroup::syncControl;
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

class ControlModeImporterStub : public ::ControlGroup::Importer
{
 public:
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return {};
  }

  bool provideActive() const override
  {
    return false;
  }
};

class ControlModeExporterMock : public ::ControlGroup::Exporter
{
 public:
  MAKE_MOCK1(takeActive, void(bool), override);
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &),
      override);
};

TEST_CASE("ControlGroup tests", "[GPU][ControlGroup]")
{
  std::string_view const id{"_id_"};
  std::vector<std::unique_ptr<IControl>> controlMocks;

  SECTION("ID is assigned")
  {
    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
    REQUIRE(ts.ID() == id);
  }

  SECTION("Pre-init its controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    REQUIRE_CALL(*controlMock, preInit(trompeloeil::_));

    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
    CommandQueueStub ctlCmds;
    ts.preInit(ctlCmds);
  }

  SECTION("Post-init its controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());
    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    REQUIRE_CALL(*controlMock, postInit(trompeloeil::_));

    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
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

      ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
      ts.init();
    }

    SECTION("Activate all inactive controls on initialization")
    {
      controlMocks.emplace_back(std::make_unique<ControlMock>());
      controlMocks.emplace_back(std::make_unique<ControlMock>());

      ControlMock *controlMock =
          static_cast<ControlMock *>(controlMocks[0].get());
      std::string const inactiveControlMockID0("_active_control_mock");
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(inactiveControlMockID0);
      ALLOW_CALL(*controlMock, active()).RETURN(true);
      FORBID_CALL(*controlMock, activate(trompeloeil::_));

      controlMock = static_cast<ControlMock *>(controlMocks[1].get());
      std::string const inactiveControlMockID1("_inactive_control_mock");
      ALLOW_CALL(*controlMock, init());
      ALLOW_CALL(*controlMock, ID()).LR_RETURN(inactiveControlMockID1);
      ALLOW_CALL(*controlMock, active()).RETURN(false);
      REQUIRE_CALL(*controlMock, activate(true));

      ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
      ts.init();
    }
  }

  SECTION("Imports controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());

    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const controlMockID("_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(true);
    REQUIRE_CALL(*controlMock, importWith(trompeloeil::_));
    REQUIRE_CALL(*controlMock, activate(true));

    ControlModeImporterStub i;
    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
    ts.init();
    ts.importControl(i);
  }

  SECTION("Export controls")
  {
    controlMocks.emplace_back(std::make_unique<ControlMock>());

    ControlMock *controlMock = static_cast<ControlMock *>(controlMocks[0].get());
    std::string const controlMockID("_control_mock_");
    ALLOW_CALL(*controlMock, init());
    ALLOW_CALL(*controlMock, ID()).LR_RETURN(controlMockID);
    ALLOW_CALL(*controlMock, active()).RETURN(true);
    REQUIRE_CALL(*controlMock, exportWith(trompeloeil::_));

    ControlModeExporterMock e;
    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
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

    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
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

    ControlGroupTestAdapter ts(id, std::move(controlMocks), true);
    ts.init();
    CommandQueueStub ctlCmds;
    ts.syncControl(ctlCmds);
  }
}
} // namespace ControlGroup
} // namespace Tests
