// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
