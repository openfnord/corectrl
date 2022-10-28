// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
