//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "cpufreqmodeprovider.h"

#include "core/components/controls/cpucontrolprovider.h"
#include "core/info/icpuinfo.h"
#include "cpufreqmode.h"

std::vector<std::unique_ptr<IControl>>
CPUFreqModeProvider::provideCPUControls(ICPUInfo const &cpuInfo,
                                        ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;
  std::vector<std::unique_ptr<IControl>> modeControls;

  for (auto &provider : cpuControlProviders()) {
    auto newControls = provider->provideCPUControls(cpuInfo, swInfo);
    modeControls.insert(modeControls.end(),
                        std::make_move_iterator(newControls.begin()),
                        std::make_move_iterator(newControls.end()));
  }

  if (!modeControls.empty())
    controls.emplace_back(std::make_unique<CPUFreqMode>(std::move(modeControls)));

  return controls;
}

std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> const &
CPUFreqModeProvider::cpuControlProviders() const
{
  return providers_();
}

bool CPUFreqModeProvider::registerProvider(
    std::unique_ptr<ICPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> &
CPUFreqModeProvider::providers_()
{
  static std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> providers;
  return providers;
}

bool const CPUFreqModeProvider::registered_ =
    CPUControlProvider::registerProvider(std::make_unique<CPUFreqModeProvider>());
