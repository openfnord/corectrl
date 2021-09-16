//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
