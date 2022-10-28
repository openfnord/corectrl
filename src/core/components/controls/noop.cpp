// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "noop.h"

Noop::Noop() noexcept
: Control(true)
, id_(Noop::ItemID)
{
}

void Noop::preInit(ICommandQueue &)
{
}

void Noop::postInit(ICommandQueue &)
{
}

void Noop::init()
{
}

std::string const &Noop::ID() const
{
  return id_;
}

void Noop::importControl(IControl::Importer &)
{
}

void Noop::exportControl(IControl::Exporter &) const
{
}

void Noop::cleanControl(ICommandQueue &)
{
}

void Noop::syncControl(ICommandQueue &)
{
}
