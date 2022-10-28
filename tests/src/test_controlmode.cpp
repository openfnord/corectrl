// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "common/commandqueuestub.h"
#include "common/controlmock.h"
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
