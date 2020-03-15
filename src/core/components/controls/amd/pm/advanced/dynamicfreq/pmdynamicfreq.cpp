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
#include "pmdynamicfreq.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMDynamicFreq::PMDynamicFreq(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept
: Control(false)
, id_(AMD::PMDynamicFreq::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
{
}

void AMD::PMDynamicFreq::preInit(ICommandQueue &)
{
}

void AMD::PMDynamicFreq::postInit(ICommandQueue &)
{
}

void AMD::PMDynamicFreq::init()
{
}

std::string const &AMD::PMDynamicFreq::ID() const
{
  return id_;
}

void AMD::PMDynamicFreq::importControl(IControl::Importer &)
{
}

void AMD::PMDynamicFreq::exportControl(IControl::Exporter &) const
{
}

void AMD::PMDynamicFreq::cleanControl(ICommandQueue &)
{
}

void AMD::PMDynamicFreq::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(dataSourceEntry_)) {
    if (dataSourceEntry_ != "auto")
      ctlCmds.add({perfLevelDataSource_->source(), "auto"});
  }
}
