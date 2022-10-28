// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpufreq.h"

#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <utility>

CPUFreq::CPUFreq(std::vector<std::string> &&scalingGovernors,
                 std::string const &defaultGovernor,
                 std::vector<std::unique_ptr<IDataSource<std::string>>>
                     &&scalingGovernorDataSources) noexcept
: Control(true)
, id_(CPUFreq::ItemID)
, scalingGovernors_(std::move(scalingGovernors))
, scalingGovernorDataSources_(std::move(scalingGovernorDataSources))
{
  scalingGovernor(defaultGovernor);
  if (scalingGovernor_.empty())
    scalingGovernor(scalingGovernors_.front());
}

void CPUFreq::preInit(ICommandQueue &)
{
}

void CPUFreq::postInit(ICommandQueue &)
{
}

void CPUFreq::init()
{
}

std::string const &CPUFreq::ID() const
{
  return id_;
}

void CPUFreq::importControl(IControl::Importer &i)
{
  auto &cpuFreqImporter = dynamic_cast<CPUFreq::Importer &>(i);
  scalingGovernor(cpuFreqImporter.provideCPUFreqScalingGovernor());
}

void CPUFreq::exportControl(IControl::Exporter &e) const
{
  auto &cpuFreqExporter = dynamic_cast<CPUFreq::Exporter &>(e);
  cpuFreqExporter.takeCPUFreqScalingGovernors(scalingGovernors());
  cpuFreqExporter.takeCPUFreqScalingGovernor(scalingGovernor());
}

void CPUFreq::cleanControl(ICommandQueue &)
{
}

void CPUFreq::syncControl(ICommandQueue &ctlCmds)
{
  for (auto &scalingGovernorDataSource : scalingGovernorDataSources_)
    if (scalingGovernorDataSource->read(dataSourceEntry_)) {
      if (dataSourceEntry_ != scalingGovernor())
        ctlCmds.add({scalingGovernorDataSource->source(), scalingGovernor()});
    }
}

std::string const &CPUFreq::scalingGovernor() const
{
  return scalingGovernor_;
}

void CPUFreq::scalingGovernor(std::string const &scalingGovernor)
{
  // only assign known scalingGovernors
  auto iter = std::find_if(scalingGovernors().cbegin(),
                           scalingGovernors().cend(),
                           [&](auto &availableScalingGovernor) {
                             return scalingGovernor == availableScalingGovernor;
                           });
  if (iter != scalingGovernors().cend())
    scalingGovernor_ = scalingGovernor;
}

std::vector<std::string> const &CPUFreq::scalingGovernors() const
{
  return scalingGovernors_;
}
