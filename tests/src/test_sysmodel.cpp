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
#include "core/info/iswinfo.h"
#include "core/isyscomponent.h"
#include "core/sysmodel.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace SysModel {

class SWInfoStub : public ISWInfo
{
 public:
  std::string info(std::string_view) const override
  {
    return "sw_info";
  }

  std::vector<std::string> keys() const override
  {
    return {"sw_info_key"};
  }

  void initialize(std::vector<std::unique_ptr<ISWInfo::IProvider>> const &) override
  {
  }
};

class SysComponentMock : public ISysComponent
{
 public:
  MAKE_MOCK0(init, void(), override);
  MAKE_MOCK1(preInit, void(ICommandQueue &), override);
  MAKE_MOCK1(postInit, void(ICommandQueue &), override);

  MAKE_CONST_MOCK0(active, bool(), override);
  MAKE_MOCK1(activate, void(bool), override);
  MAKE_CONST_MOCK0(key, std::string const &(), override);

  // trompeloeil fails to mock this method
  std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
  componentInfo() const override
  {
    std::vector<std::pair<std::string, std::string>> cInfo;
    cInfo.emplace_back("c_info_key", "c_info");
    return {"component", std::move(cInfo)};
  }

  MAKE_MOCK1(sync, void(ICommandQueue &), override);
  MAKE_MOCK1(updateSensors,
             void(std::unordered_map<std::string,
                                     std::unordered_set<std::string>> const &),
             override);
  MAKE_CONST_MOCK0(ID, std::string const &(), override);
  MAKE_MOCK1(importWith, void(Importable::Importer &), override);
  MAKE_CONST_MOCK1(exportWith, void(Exportable::Exporter &), override);
};

class SysModelImporterStub : public ISysModel::Importer
{
 public:
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &) override
  {
    return *this;
  }
};

class SysModelExporterStub : public ISysModel::Exporter
{
 public:
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return *this;
  }
};

TEST_CASE("SysModel tests", "[SysModel]")
{
  CommandQueueStub ctlCmds;

  auto info = std::make_unique<SWInfoStub>();

  std::vector<std::unique_ptr<ISysComponent>> components;
  components.emplace_back(std::make_unique<SysComponentMock>());
  auto &componentMock = static_cast<SysComponentMock &>(*components[0]);

  ::SysModel ts(std::move(info), std::move(components));

  SECTION("Has ISysModel ID")
  {
    REQUIRE(ts.ID() == ISysModel::ItemID);
  }

  SECTION("Pre-init components")
  {
    REQUIRE_CALL(componentMock, preInit(trompeloeil::_));
    ts.preInit(ctlCmds);
  }

  SECTION("Init components")
  {
    REQUIRE_CALL(componentMock, init());
    ts.init();
  }

  SECTION("Post-init components")
  {
    REQUIRE_CALL(componentMock, postInit(trompeloeil::_));
    ts.postInit(ctlCmds);
  }

  SECTION("Update component's sensors")
  {
    std::unordered_map<std::string, std::unordered_set<std::string>> ignored;
    REQUIRE_CALL(componentMock, updateSensors(trompeloeil::_))
        .LR_WITH(&_1 == &ignored);
    ts.updateSensors(ignored);
  }

  SECTION("Sync components")
  {
    REQUIRE_CALL(componentMock, sync(trompeloeil::_));
    ts.sync(ctlCmds);
  }

  SECTION("System info can be retrieved")
  {
    auto sysInfo = ts.info();

    REQUIRE_FALSE(sysInfo.empty());

    SECTION("Software info is always present")
    {
      std::vector<std::pair<std::string, std::string>> swInfoData;
      swInfoData.emplace_back("sw_info_key", "sw_info");
      auto swInfo = std::make_pair(std::string("Software"),
                                   std::move(swInfoData));

      REQUIRE_THAT(sysInfo, Catch::VectorContains(swInfo));
    }

    SECTION("Has components info")
    {
      auto compInfo = componentMock.componentInfo();

      REQUIRE_THAT(sysInfo, Catch::VectorContains(compInfo));
    }
  }

  SECTION("Imports its components")
  {
    REQUIRE_CALL(componentMock, importWith(trompeloeil::_));

    SysModelImporterStub i;
    ts.importWith(i);
  }

  SECTION("Exports its components")
  {
    REQUIRE_CALL(componentMock, exportWith(trompeloeil::_));

    SysModelExporterStub e;
    ts.exportWith(e);
  }
}
} // namespace SysModel
} // namespace Tests
