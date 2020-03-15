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
