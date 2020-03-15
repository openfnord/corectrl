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
