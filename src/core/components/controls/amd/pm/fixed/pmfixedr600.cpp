// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedr600.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <utility>

AMD::PMFixedR600::PMFixedR600(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource) noexcept
: AMD::PMFixed(AMD::PMFixedR600::PerfLevel::low)
, perfLevelDataSource_(std::move(perfLevelDataSource))
{
}

void AMD::PMFixedR600::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({perfLevelDataSource_->source(),
               std::string(AMD::PMFixedR600::PerfLevel::clean)});
}

void AMD::PMFixedR600::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_)) {
    if (perfLevelEntry_ != mode())
      ctlCmds.add({perfLevelDataSource_->source(), mode()});
  }
}

std::vector<std::string> const &AMD::PMFixedR600::modes() const
{
  return modes_;
}

std::vector<std::string> const AMD::PMFixedR600::modes_{
    std::string(AMD::PMFixedR600::PerfLevel::low),
    std::string(AMD::PMFixedR600::PerfLevel::high)};
