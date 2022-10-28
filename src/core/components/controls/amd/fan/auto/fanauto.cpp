// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanauto.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::FanAuto::FanAuto(std::unique_ptr<IDataSource<unsigned int>> &&dataSource) noexcept
: Control(true)
, id_(AMD::FanAuto::ItemID)
, dataSource_(std::move(dataSource))
{
}

void AMD::FanAuto::preInit(ICommandQueue &)
{
}

void AMD::FanAuto::postInit(ICommandQueue &)
{
}

void AMD::FanAuto::init()
{
}

std::string const &AMD::FanAuto::ID() const
{
  return id_;
}

void AMD::FanAuto::importControl(IControl::Importer &)
{
}

void AMD::FanAuto::exportControl(IControl::Exporter &) const
{
}

void AMD::FanAuto::cleanControl(ICommandQueue &)
{
}

void AMD::FanAuto::syncControl(ICommandQueue &ctlCmds)
{
  if (dataSource_->read(pwmEnable_)) {
    if (pwmEnable_ != 2)
      ctlCmds.add({dataSource_->source(), "2"});
  }
}
