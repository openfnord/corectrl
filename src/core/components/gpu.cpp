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
#include "gpu.h"

#include "controls/icontrol.h"
#include "core/info/igpuinfo.h"
#include "sensors/isensor.h"
#include <utility>

GPU::GPU(std::unique_ptr<IGPUInfo> &&info,
         std::vector<std::unique_ptr<IControl>> &&controls,
         std::vector<std::unique_ptr<ISensor>> &&sensors) noexcept
: id_(IGPU::ItemID)
, info_(std::move(info))
, controls_(std::move(controls))
, sensors_(std::move(sensors))
{
  key_ = "GPU" + std::to_string(info_->index());
}

bool GPU::active() const
{
  return active_;
}

void GPU::activate(bool active)
{
  active_ = active;
}

std::string const &GPU::key() const
{
  return key_;
}

std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
GPU::componentInfo() const
{
  std::pair<std::string, std::vector<std::pair<std::string, std::string>>> info;

  auto name(info_->info(IGPUInfo::Keys::subdeviceName));
  if (!name.empty())
    name.append("\n");
  name.append("[GPU ").append(std::to_string(info_->index())).append("]");
  info.first = name;

  auto infoKeys = info_->keys();
  for (auto &infoKey : infoKeys)
    info.second.emplace_back(infoKey, info_->info(infoKey));

  return info;
}

void GPU::init()
{
  for (auto &control : controls_)
    control->init();
}

void GPU::preInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->preInit(ctlCmds);
}

void GPU::postInit(ICommandQueue &ctlCmds)
{
  for (auto &control : controls_)
    control->postInit(ctlCmds);
}

void GPU::sync(ICommandQueue &ctlCmds)
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

void GPU::updateSensors()
{
  for (auto &sensor : sensors_)
    sensor->update();
}

IGPUInfo const &GPU::info() const
{
  return *info_;
}

std::string const &GPU::ID() const
{
  return id_;
}

void GPU::importWith(Importable::Importer &i)
{
  auto importer = i.provideImporter(*this);
  if (importer.has_value()) {
    auto &gpuImporter = dynamic_cast<IGPU::Importer &>(importer->get());

    activate(gpuImporter.provideActive());

    for (auto &control : controls_)
      control->importWith(*importer);
  }
}

void GPU::exportWith(Exportable::Exporter &e) const
{
  auto exporter = e.provideExporter(*this);
  if (exporter.has_value()) {
    auto &gpuExporter = dynamic_cast<IGPU::Exporter &>(exporter->get());

    gpuExporter.takeActive(active());
    gpuExporter.takeInfo(info());

    for (auto &sensor : sensors_)
      gpuExporter.takeSensor(*sensor);

    for (auto &control : controls_)
      control->exportWith(*exporter);
  }
}
