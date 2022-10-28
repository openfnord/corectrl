// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmdynamicfreq.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMDynamicFreq::PMDynamicFreq(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept
: Control(false)
, id_(AMD::PMDynamicFreq::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
{
}

void AMD::PMDynamicFreq::preInit(ICommandQueue &)
{
}

void AMD::PMDynamicFreq::postInit(ICommandQueue &)
{
}

void AMD::PMDynamicFreq::init()
{
}

std::string const &AMD::PMDynamicFreq::ID() const
{
  return id_;
}

void AMD::PMDynamicFreq::importControl(IControl::Importer &)
{
}

void AMD::PMDynamicFreq::exportControl(IControl::Exporter &) const
{
}

void AMD::PMDynamicFreq::cleanControl(ICommandQueue &)
{
}

void AMD::PMDynamicFreq::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(dataSourceEntry_)) {
    if (dataSourceEntry_ != "auto")
      ctlCmds.add({perfLevelDataSource_->source(), "auto"});
  }
}
