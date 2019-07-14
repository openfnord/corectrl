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
#include "pmpowerprofile.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"

AMD::PMPowerProfile::PMPowerProfile(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>>>
        &&powerProfileDataSource) noexcept
: Control(true)
, id_(AMD::PMPowerProfile::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
, powerProfileDataSource_(std::move(powerProfileDataSource))
, currentModeIndex_(-1)
, defaultModeIndex_(-1)
{
  if (powerProfileDataSource_->read(dataSourceLines_)) {

    auto modes = Utils::AMD::parsePowerProfileModeModes(dataSourceLines_);
    if (modes.has_value()) {
      for (auto &[mode, index] : modes.value()) {
        modes_.push_back(mode);
        indexMode_.emplace(index, mode);
      }

      if (modes_.empty())
        indexMode_.emplace(currentModeIndex_, "");
      else {
        mode(modes_.front());
        defaultModeIndex_ = currentModeIndex_;
      }
    }
  }
}

void AMD::PMPowerProfile::preInit(ICommandQueue &ctlCmds)
{
  cleanControl(ctlCmds);
}

void AMD::PMPowerProfile::init()
{
}

std::string const &AMD::PMPowerProfile::ID() const
{
  return id_;
}

void AMD::PMPowerProfile::importControl(IControl::Importer &i)
{
  auto &pmPowerProfileImporter = dynamic_cast<AMD::PMPowerProfile::Importer &>(i);
  mode(pmPowerProfileImporter.providePMPowerProfileMode());
}

void AMD::PMPowerProfile::exportControl(IControl::Exporter &e) const
{
  auto &pmPowerProfileExporter = dynamic_cast<AMD::PMPowerProfile::Exporter &>(e);
  pmPowerProfileExporter.takePMPowerProfileModes(modes());
  pmPowerProfileExporter.takePMPowerProfileMode(mode());
}

void AMD::PMPowerProfile::cleanControl(ICommandQueue &ctlCmds)
{
  if (!modes_.empty()) {
    ctlCmds.add({perfLevelDataSource_->source(), "manual"});
    ctlCmds.add(
        {powerProfileDataSource_->source(), std::to_string(defaultModeIndex_)});
  }
}

void AMD::PMPowerProfile::syncControl(ICommandQueue &ctlCmds)
{
  if (!modes_.empty() && perfLevelDataSource_->read(dataSourceEntry_) &&
      powerProfileDataSource_->read(dataSourceLines_)) {

    if (dataSourceEntry_ != "manual") {
      ctlCmds.add({perfLevelDataSource_->source(), "manual"});
      ctlCmds.add({powerProfileDataSource_->source(),
                   std::to_string(currentModeIndex_)});
    }
    else {
      auto modeIndex =
          Utils::AMD::parsePowerProfileModeCurrentModeIndex(dataSourceLines_);
      if (modeIndex.has_value() && currentModeIndex_ != modeIndex)
        ctlCmds.add({powerProfileDataSource_->source(),
                     std::to_string(currentModeIndex_)});
    }
  }
}

void AMD::PMPowerProfile::mode(std::string const &mode)
{
  for (auto &kv : indexMode_) {
    if (kv.second == mode) {
      currentModeIndex_ = kv.first;
      break;
    }
  }
}

std::string const &AMD::PMPowerProfile::mode() const
{

  return indexMode_.at(currentModeIndex_);
}

std::vector<std::string> const &AMD::PMPowerProfile::modes() const
{
  return modes_;
}
