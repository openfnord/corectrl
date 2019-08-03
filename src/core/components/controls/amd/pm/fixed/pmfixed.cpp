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
#include "pmfixed.h"

#include <algorithm>

AMD::PMFixed::PMFixed(std::string_view mode) noexcept
: Control(false)
, id_(AMD::PMFixed::ItemID)
, mode_(mode)
{
}

void AMD::PMFixed::preInit(ICommandQueue &)
{
}

void AMD::PMFixed::postInit(ICommandQueue &)
{
}

void AMD::PMFixed::init()
{
}

std::string const &AMD::PMFixed::ID() const
{
  return id_;
}

void AMD::PMFixed::importControl(IControl::Importer &i)
{
  auto &pmFixedImporter = dynamic_cast<AMD::PMFixed::Importer &>(i);
  mode(pmFixedImporter.providePMFixedMode());
}

void AMD::PMFixed::exportControl(IControl::Exporter &e) const
{
  auto &pmFixedExporter = dynamic_cast<AMD::PMFixed::Exporter &>(e);
  pmFixedExporter.takePMFixedModes(modes());
  pmFixedExporter.takePMFixedMode(mode());
}

std::string const &AMD::PMFixed::mode() const
{
  return mode_;
}

void AMD::PMFixed::mode(std::string const &mode)
{
  // only assign known modes
  auto iter = std::find_if(
      modes().cbegin(), modes().cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != modes().cend())
    mode_ = mode;
}
