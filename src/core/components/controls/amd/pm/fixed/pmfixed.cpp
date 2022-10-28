// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
