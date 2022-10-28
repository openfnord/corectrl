// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedlegacy.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMFixedLegacy::PMFixedLegacy(
    std::unique_ptr<IDataSource<std::string>> &&powerMethodDataSource,
    std::unique_ptr<IDataSource<std::string>> &&powerProfileDataSource) noexcept
: AMD::PMFixed(AMD::PMFixedLegacy::Profile::low)
, powerMethodDataSource_(std::move(powerMethodDataSource))
, powerProfileDataSource_(std::move(powerProfileDataSource))
{
}

void AMD::PMFixedLegacy::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({powerMethodDataSource_->source(),
               std::string(AMD::PMFixedLegacy::Method)});
  ctlCmds.add({powerProfileDataSource_->source(),
               std::string(AMD::PMFixedLegacy::Profile::clean)});
}

void AMD::PMFixedLegacy::syncControl(ICommandQueue &ctlCmds)
{
  if (powerMethodDataSource_->read(powerMethodEntry_) &&
      powerProfileDataSource_->read(powerProfileEntry_)) {

    if (powerMethodEntry_ != AMD::PMFixedLegacy::Method) {
      ctlCmds.add({powerMethodDataSource_->source(),
                   std::string(AMD::PMFixedLegacy::Method)});
      ctlCmds.add({powerProfileDataSource_->source(), mode()});
    }
    else if (powerProfileEntry_ != mode()) {
      ctlCmds.add({powerProfileDataSource_->source(), mode()});
    }
  }
}

std::vector<std::string> const &AMD::PMFixedLegacy::modes() const
{
  return modes_;
}

std::vector<std::string> const AMD::PMFixedLegacy::modes_{
    std::string(AMD::PMFixedLegacy::Profile::low),
    std::string(AMD::PMFixedLegacy::Profile::mid),
    std::string(AMD::PMFixedLegacy::Profile::high)};
