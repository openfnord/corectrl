// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
