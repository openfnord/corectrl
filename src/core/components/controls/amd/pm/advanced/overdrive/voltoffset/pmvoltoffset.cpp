// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmvoltoffset.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <utility>

AMD::PMVoltOffset::PMVoltOffset(
    std::unique_ptr<IDataSource<std::vector<std::string>>>
        &&ppOdClkVoltDataSource) noexcept
: Control(true)
, id_(AMD::PMVoltOffset::ItemID)
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
, range_(std::make_pair(units::voltage::millivolt_t(-250),
                        units::voltage::millivolt_t(250)))
{
}

void AMD::PMVoltOffset::preInit(ICommandQueue &)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    preInitOffset_ =
        Utils::AMD::parseOverdriveVoltOffset(ppOdClkVoltLines_).value();
  }
}

void AMD::PMVoltOffset::postInit(ICommandQueue &ctlCmds)
{
  ctlCmds.add({ppOdClkVoltDataSource_->source(), ppOdClkVoltCmd(preInitOffset_)});
}

void AMD::PMVoltOffset::init()
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    value_ = Utils::AMD::parseOverdriveVoltOffset(ppOdClkVoltLines_).value();
  }
}

std::string const &AMD::PMVoltOffset::ID() const
{
  return id_;
}

void AMD::PMVoltOffset::importControl(IControl::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMVoltOffset::Importer &>(i);
  value(importer.providePMVoltOffsetValue());
}

void AMD::PMVoltOffset::exportControl(IControl::Exporter &e) const
{
  auto &exporter = dynamic_cast<AMD::PMVoltOffset::Exporter &>(e);

  auto [mim, max] = range();
  exporter.takePMVoltOffsetRange(mim, max);
  exporter.takePMVoltOffsetValue(value());
}

void AMD::PMVoltOffset::cleanControl(ICommandQueue &)
{
}

void AMD::PMVoltOffset::syncControl(ICommandQueue &ctlCmds)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    auto offset = Utils::AMD::parseOverdriveVoltOffset(ppOdClkVoltLines_).value();
    if (offset != value()) {
      ctlCmds.add({ppOdClkVoltDataSource_->source(), ppOdClkVoltCmd(value())});
    }
  }
}

std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const &
AMD::PMVoltOffset::range() const
{
  return range_;
}

units::voltage::millivolt_t AMD::PMVoltOffset::value() const
{
  return value_;
}

void AMD::PMVoltOffset::value(units::voltage::millivolt_t value)
{
  value_ = std::clamp(value, range_.first, range_.second);
}

std::string
AMD::PMVoltOffset::ppOdClkVoltCmd(units::voltage::millivolt_t offset) const
{
  std::string cmd;
  cmd.reserve(8);
  cmd.append("vo ").append(std::to_string(offset.to<int>()));
  return cmd;
}
