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
#include "pmauto.h"

AMD::PMAuto::PMAuto() noexcept
: Control(true)
, id_(AMD::PMAuto::ItemID)
{
}

void AMD::PMAuto::preInit(ICommandQueue &)
{
}

void AMD::PMAuto::postInit(ICommandQueue &)
{
}

void AMD::PMAuto::init()
{
}

std::string const &AMD::PMAuto::ID() const
{
  return id_;
}

void AMD::PMAuto::importControl(IControl::Importer &)
{
}

void AMD::PMAuto::exportControl(IControl::Exporter &) const
{
}
