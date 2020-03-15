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
#include "cpu.h"

#include "controls/icontrol.h"
#include "core/info/icpuinfo.h"
#include "sensors/isensor.h"
#include <utility>

CPU::CPU(std::unique_ptr<ICPUInfo> &&info,
         std::vector<std::unique_ptr<IControl>> &&controls,
         std::vector<std::unique_ptr<ISensor>> &&sensors) noexcept
: id_(ICPU::ItemID)
, info_(std::move(info))
, controls_(std::move(controls))
, sensors_(std::move(sensors))
{
  key_ = "CPU" + std::to_string(info_->socketId());
}

bool CPU::active() const
{
  return active_;
}

void CPU::activate(bool active)
{
  active_ = active;
}

std::string const &CPU::key() const
{
  return key_;
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
CPU::componentInfo() const
{
  std::pair<std::string, std::vector<std::pair<std::string, std::string>>> info;

  auto name(info_->info(ICPUInfo::Keys::modelName));
  if (!name.empty())
    name.append("\n");
  name.append("[CPU ").append(std::to_string(info_->socketId())).append("]");
  info.first = name;

  auto infoKeys = info_->keys();
  for (auto &infoKey : infoKeys)
    info.second.emplace_back(infoKey, info_->info(infoKey));

  return info;
}

void CPU::init()
{
  for (auto &control : controls_)
    control->init();
}

void CPU::preInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->preInit(ctlCmds);
}

void CPU::postInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->postInit(ctlCmds);
}

void CPU::sync(ICommandQueue &ctlCmds)
{
  if (active_) {
    // NOTE clean and sync commands generation cannot be interleaved.
    // Interleaving them could cause conflicts between clean and sync
    // commands of different controls, leading to an incorrect hardware
    // state.
    for (auto &control : controls_)
      control->clean(ctlCmds);
    for (auto &control : controls_)
      control->sync(ctlCmds);
  }
}

void CPU::updateSensors(
    std::unordered_map<std::string, std::unordered_set<std::string>> const &ignored)
{
  for (auto &sensor : sensors_) {
    if (ignored.count(key_) > 0 && ignored.at(key_).count(sensor->ID()) > 0)
      continue; // skip ignored sensors

    sensor->update();
  }
}

ICPUInfo const &CPU::info() const
{
  return *info_;
}

std::string const &CPU::ID() const
{
  return id_;
}

void CPU::importWith(Importable::Importer &i)
{
  auto importer = i.provideImporter(*this);
  if (importer.has_value()) {
    auto &cpuImporter = dynamic_cast<ICPU::Importer &>(importer->get());

    activate(cpuImporter.provideActive());

    for (auto &control : controls_)
      control->importWith(*importer);
  }
}

void CPU::exportWith(Exportable::Exporter &e) const
{
  auto exporter = e.provideExporter(*this);
  if (exporter.has_value()) {
    auto &cpuExporter = dynamic_cast<ICPU::Exporter &>(exporter->get());

    cpuExporter.takeActive(active());
    cpuExporter.takeInfo(info());

    for (auto &sensor : sensors_)
      cpuExporter.takeSensor(*sensor);

    for (auto &control : controls_)
      control->exportWith(*exporter);
  }
}
