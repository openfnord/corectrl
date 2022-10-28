// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerprofile.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"

AMD::PMPowerProfile::PMPowerProfile(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&powerProfileDataSource,
    std::vector<std::pair<std::string, int>> const &modes) noexcept
: Control(true)
, id_(AMD::PMPowerProfile::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
, powerProfileDataSource_(std::move(powerProfileDataSource))
{
  for (auto &[mode, index] : modes) {
    modes_.push_back(mode);
    indexMode_.emplace(index, mode);
  }

  mode(modes_.front());
  defaultModeIndex_ = currentModeIndex_;
}

void AMD::PMPowerProfile::preInit(ICommandQueue &)
{
}

void AMD::PMPowerProfile::postInit(ICommandQueue &)
{
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
  if (perfLevelDataSource_->read(dataSourceEntry_) &&
      dataSourceEntry_ != "manual")
    ctlCmds.add({perfLevelDataSource_->source(), "manual"});

  ctlCmds.add(
      {powerProfileDataSource_->source(), std::to_string(defaultModeIndex_)});
}

void AMD::PMPowerProfile::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(dataSourceEntry_) &&
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
