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
#include "pmautor600.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMAutoR600::PMAutoR600(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept
: perfLevelDataSource_(std::move(perfLevelDataSource))
{
}

void AMD::PMAutoR600::cleanControl(ICommandQueue &)
{
}

void AMD::PMAutoR600::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_)) {
    if (perfLevelEntry_ != AMD::PMAutoR600::PerfLevel)
      ctlCmds.add({perfLevelDataSource_->source(),
                   std::string(AMD::PMAutoR600::PerfLevel)});
  }
}
