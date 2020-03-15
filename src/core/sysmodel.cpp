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
#include "sysmodel.h"

#include "core/exportable.h"
#include "core/importable.h"
#include "core/isyscomponent.h"
#include "info/iswinfo.h"

SysModel::SysModel(std::shared_ptr<ISWInfo> swInfo,
                   std::vector<std::unique_ptr<ISysComponent>> &&components) noexcept
: id_{ISysModel::ItemID}
, swInfo_(std::move(swInfo))
, components_(std::move(components))
{
}

void SysModel::init()
{
  for (auto &component : components_)
    component->init();
}

void SysModel::preInit(ICommandQueue &ctlCmds)
{
  for (auto &component : components_)
    component->preInit(ctlCmds);
}

void SysModel::postInit(ICommandQueue &ctlCmds)
{
  for (auto &component : components_)
    component->postInit(ctlCmds);
}

void SysModel::sync(ICommandQueue &ctlCmds)
{
  for (auto &component : components_)
    component->sync(ctlCmds);
}

void SysModel::updateSensors(
    std::unordered_map<std::string, std::unordered_set<std::string>> const &ignored)
{
  for (auto &component : components_)
    component->updateSensors(ignored);
}

std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
SysModel::info() const
{
  std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
      info;

  info.emplace_back(softwareInfo());

  for (auto &component : components_)
    info.emplace_back(component->componentInfo());

  return info;
}

std::string const &SysModel::ID() const
{
  return id_;
}

void SysModel::importWith(Importable::Importer &i)
{
  auto importer = i.provideImporter(*this);
  if (importer.has_value()) {
    for (auto &component : components_)
      component->importWith(*importer);
  }
}

void SysModel::exportWith(Exportable::Exporter &e) const
{
  auto exporter = e.provideExporter(*this);
  if (exporter.has_value()) {
    for (auto &component : components_)
      component->exportWith(*exporter);
  }
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
SysModel::softwareInfo() const
{
  std::pair<std::string, std::vector<std::pair<std::string, std::string>>> info;

  info.first = "Software";

  auto swInfoKeys = swInfo_->keys();
  for (auto &swInfoKey : swInfoKeys)
    info.second.emplace_back(swInfoKey, swInfo_->info(swInfoKey));

  return info;
}
