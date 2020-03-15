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
#include "pmfixedr600.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMFixedR600::PMFixedR600(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept
: AMD::PMFixed(AMD::PMFixedR600::PerfLevel::low)
, perfLevelDataSource_(std::move(perfLevelDataSource))
{
}

void AMD::PMFixedR600::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({perfLevelDataSource_->source(),
               std::string(AMD::PMFixedR600::PerfLevel::clean)});
}

void AMD::PMFixedR600::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_)) {
    if (perfLevelEntry_ != mode())
      ctlCmds.add({perfLevelDataSource_->source(), mode()});
  }
}

std::vector<std::string> const &AMD::PMFixedR600::modes() const
{
  return modes_;
}

std::vector<std::string> const AMD::PMFixedR600::modes_{
    std::string(AMD::PMFixedR600::PerfLevel::low),
    std::string(AMD::PMFixedR600::PerfLevel::high)};
