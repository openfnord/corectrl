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
#include "pmpowerstate.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <utility>

AMD::PMPowerState::PMPowerState(
    std::unique_ptr<IDataSource<std::string>> &&powerDpmStateDataSource) noexcept
: Control(true)
, id_(AMD::PMPowerState::ItemID)
, powerDpmStateDataSource_(std::move(powerDpmStateDataSource))
, mode_(State::Balanced)
{
}

void AMD::PMPowerState::preInit(ICommandQueue &)
{
}

void AMD::PMPowerState::postInit(ICommandQueue &)
{
}

void AMD::PMPowerState::init()
{
}

std::string const &AMD::PMPowerState::ID() const
{
  return id_;
}

void AMD::PMPowerState::importControl(IControl::Importer &i)
{
  auto &pmPowerStateImporter = dynamic_cast<AMD::PMPowerState::Importer &>(i);
  mode(pmPowerStateImporter.providePMPowerStateMode());
}

void AMD::PMPowerState::exportControl(IControl::Exporter &e) const
{
  auto &pmPowerStateExporter = dynamic_cast<AMD::PMPowerState::Exporter &>(e);
  pmPowerStateExporter.takePMPowerStateModes(modes());
  pmPowerStateExporter.takePMPowerStateMode(mode());
}

void AMD::PMPowerState::cleanControl(ICommandQueue &)
{
}

void AMD::PMPowerState::syncControl(ICommandQueue &ctlCmds)
{
  if (powerDpmStateDataSource_->read(powerDpmStateEntry_)) {
    if (powerDpmStateEntry_ != mode())
      ctlCmds.add({powerDpmStateDataSource_->source(), mode()});
  }
}

std::string const &AMD::PMPowerState::mode() const
{
  return mode_;
}

void AMD::PMPowerState::mode(std::string const &mode)
{
  // only assign known modes
  auto iter = std::find_if(
      modes().cbegin(), modes().cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != modes().cend())
    mode_ = mode;
}

std::vector<std::string> const &AMD::PMPowerState::modes() const
{
  return modes_;
}

std::vector<std::string> const AMD::PMPowerState::modes_{
    std::string(AMD::PMPowerState::State::Battery),
    std::string(AMD::PMPowerState::State::Balanced),
    std::string(AMD::PMPowerState::State::Performance)};
