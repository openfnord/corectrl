// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanfixed.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

AMD::FanFixed::FanFixed(
    std::unique_ptr<IDataSource<unsigned int>> &&pwmEnableDataSource,
    std::unique_ptr<IDataSource<unsigned int>> &&pwmDataSource) noexcept
: Control(false)
, id_(AMD::FanFixed::ItemID)
, pwmEnableDataSource_(std::move(pwmEnableDataSource))
, pwmDataSource_(std::move(pwmDataSource))
, value_(64)
, lastValue_(std::numeric_limits<unsigned int>::max())
, fanStop_(false)
, fanStartValue_(54)
{
}

void AMD::FanFixed::preInit(ICommandQueue &)
{
}

void AMD::FanFixed::postInit(ICommandQueue &)
{
}

void AMD::FanFixed::init()
{
}

std::string const &AMD::FanFixed::ID() const
{
  return id_;
}

void AMD::FanFixed::importControl(IControl::Importer &i)
{
  auto &fanFixedImporter = dynamic_cast<AMD::FanFixed::Importer &>(i);
  value(static_cast<unsigned int>(
      std::round(fanFixedImporter.provideFanFixedValue().to<double>() * 255)));
  fanStop(fanFixedImporter.provideFanFixedFanStop());
  fanStartValue(static_cast<unsigned int>(std::round(
      fanFixedImporter.provideFanFixedFanStartValue().to<double>() * 255)));
}

void AMD::FanFixed::exportControl(IControl::Exporter &e) const
{
  auto &fanFixedExporter = dynamic_cast<AMD::FanFixed::Exporter &>(e);
  fanFixedExporter.takeFanFixedValue(std::round(value() / 2.55));
  fanFixedExporter.takeFanFixedFanStop(fanStop());
  fanFixedExporter.takeFanFixedFanStartValue(std::round(fanStartValue() / 2.55));
}

void AMD::FanFixed::cleanControl(ICommandQueue &)
{
  lastValue_ = std::numeric_limits<unsigned int>::max();
}

void AMD::FanFixed::syncControl(ICommandQueue &ctlCmds)
{
  if (pwmEnableDataSource_->read(pwmEnable_) && pwmDataSource_->read(pwm_)) {
    auto pwmValue = (fanStop() && value() < fanStartValue()) ? 0 : value();
    if (pwmEnable_ != 1) {
      ctlCmds.add({pwmEnableDataSource_->source(), "1"});
      ctlCmds.add({pwmDataSource_->source(), std::to_string(pwmValue)});
    }
    // NOTE pwm_ must be checked as well. The value that the hardware uses
    // doesn't have to match the assigned value through pwm1.
    // See: https://bugs.freedesktop.org/show_bug.cgi?id=110213
    else if (pwm_ != pwmValue && lastValue_ != pwmValue) {
      ctlCmds.add({pwmDataSource_->source(), std::to_string(pwmValue)});
    }
    lastValue_ = pwmValue;
  }
}

unsigned int AMD::FanFixed::value() const
{
  return value_;
}

void AMD::FanFixed::value(unsigned int value)
{
  value_ = static_cast<unsigned int>(std::clamp(value, 0u, 255u));
}

bool AMD::FanFixed::fanStop() const
{
  return fanStop_;
}

void AMD::FanFixed::fanStop(bool enable)
{
  fanStop_ = enable;
}

unsigned int AMD::FanFixed::fanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanFixed::fanStartValue(unsigned int value)
{
  fanStartValue_ = static_cast<unsigned int>(std::clamp(value, 0u, 255u));
}
