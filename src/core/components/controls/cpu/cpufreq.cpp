//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
