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
#include "fanauto.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::FanAuto::FanAuto(std::unique_ptr<IDataSource<unsigned int>> &&dataSource) noexcept
: Control(true)
, id_(AMD::FanAuto::ItemID)
, dataSource_(std::move(dataSource))
{
}

void AMD::FanAuto::preInit(ICommandQueue &)
{
}

void AMD::FanAuto::postInit(ICommandQueue &)
{
}

void AMD::FanAuto::init()
{
}

std::string const &AMD::FanAuto::ID() const
{
  return id_;
}

void AMD::FanAuto::importControl(IControl::Importer &)
{
}

void AMD::FanAuto::exportControl(IControl::Exporter &) const
{
}

void AMD::FanAuto::cleanControl(ICommandQueue &)
{
}

void AMD::FanAuto::syncControl(ICommandQueue &ctlCmds)
{
  if (dataSource_->read(pwmEnable_)) {
    if (pwmEnable_ != 2)
      ctlCmds.add({dataSource_->source(), "2"});
  }
}
