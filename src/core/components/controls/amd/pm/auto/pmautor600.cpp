// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmautor600.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMAutoR600::PMAutoR600(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept
: perfLevelDataSource_(std::move(perfLevelDataSource))
{
}

void AMD::PMAutoR600::cleanControl(ICommandQueue &)
{
}

void AMD::PMAutoR600::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_)) {
    if (perfLevelEntry_ != AMD::PMAutoR600::PerfLevel)
      ctlCmds.add({perfLevelDataSource_->source(),
                   std::string(AMD::PMAutoR600::PerfLevel)});
  }
}
