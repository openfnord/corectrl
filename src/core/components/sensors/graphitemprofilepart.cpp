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
#include "graphitemprofilepart.h"

#include <memory>

GraphItemProfilePart::GraphItemProfilePart(std::string_view id,
                                           std::string_view color) noexcept
: id_(id)
, color_(color)
{
}

std::unique_ptr<Exportable::Exporter>
GraphItemProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> GraphItemProfilePart::initializer()
{
  return nullptr;
}

std::string const &GraphItemProfilePart::ID() const
{
  return id_;
}

void GraphItemProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &itemImporter = dynamic_cast<GraphItemProfilePart::Importer &>(i);
  color_ = itemImporter.provideColor();
}

void GraphItemProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &itemExporter = dynamic_cast<GraphItemProfilePart::Exporter &>(e);
  itemExporter.takeColor(color_);
}

std::unique_ptr<IProfilePart> GraphItemProfilePart::cloneProfilePart() const
{
  return std::make_unique<GraphItemProfilePart>(id_, color_);
}
