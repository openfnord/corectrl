// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmautolegacy.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMAutoLegacy::PMAutoLegacy(
    std::unique_ptr<IDataSource<std::string>> &&powerMethodDataSource,
    std::unique_ptr<IDataSource<std::string>> &&powerProfileDataSource) noexcept
: powerMethodDataSource_(std::move(powerMethodDataSource))
, powerProfileDataSource_(std::move(powerProfileDataSource))
{
}

void AMD::PMAutoLegacy::cleanControl(ICommandQueue &)
{
}

void AMD::PMAutoLegacy::syncControl(ICommandQueue &ctlCmds)
{
  if (powerMethodDataSource_->read(powerMethodEntry_) &&
      powerProfileDataSource_->read(powerProfileEntry_)) {
    if (powerMethodEntry_ != AMD::PMAutoLegacy::Method ||
        powerProfileEntry_ != AMD::PMAutoLegacy::Profile) {
      ctlCmds.add({powerMethodDataSource_->source(),
                   std::string(AMD::PMAutoLegacy::Method)});
      ctlCmds.add({powerProfileDataSource_->source(),
                   std::string(AMD::PMAutoLegacy::Profile)});
    }
  }
}
