// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmoverdrive.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMOverdrive::PMOverdrive(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&ppOdClkVoltDataSource,
    std::vector<std::unique_ptr<IControl>> &&controls) noexcept
: ControlGroup(AMD::PMOverdrive::ItemID, std::move(controls), false)
, perfLevelDataSource_(std::move(perfLevelDataSource))
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
{
}

void AMD::PMOverdrive::preInit(ICommandQueue &ctlCmds)
{
  perfLevelDataSource_->read(perfLevelPreInitValue_);

  // NOTE Each aggregated control will generate clean commands on its
  // own preInit stage. As cleanControl forces the generation of clean
  // commands on aggregated controls, it cannot be called here.

  if (perfLevelDataSource_->read(perfLevelEntry_) &&
      perfLevelEntry_ != "manual")
    ctlCmds.add({perfLevelDataSource_->source(), "manual"});

  ctlCmds.add({ppOdClkVoltDataSource_->source(), "r"});
  ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

  ControlGroup::preInit(ctlCmds);
}

void AMD::PMOverdrive::postInit(ICommandQueue &ctlCmds)
{
  ControlGroup::postInit(ctlCmds);

  ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});
  ctlCmds.add({perfLevelDataSource_->source(), perfLevelPreInitValue_});
}

void AMD::PMOverdrive::cleanControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_) &&
      perfLevelEntry_ != "manual")
    ctlCmds.add({perfLevelDataSource_->source(), "manual"});

  ctlCmds.add({ppOdClkVoltDataSource_->source(), "r"});
  ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

  ControlGroup::cleanControl(ctlCmds);
}

void AMD::PMOverdrive::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_)) {

    if (perfLevelEntry_ != "manual")
      ctlCmds.add({perfLevelDataSource_->source(), "manual"});

    ctlCmds.pack(true);

    ControlGroup::syncControl(ctlCmds);

    auto commit = ctlCmds.packWritesTo(ppOdClkVoltDataSource_->source());
    if (commit.has_value() && *commit)
      ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});

    ctlCmds.pack(false);
  }
}

std::string const &AMD::PMOverdrive::perfLevelPreInitValue() const
{
  return perfLevelPreInitValue_;
}
