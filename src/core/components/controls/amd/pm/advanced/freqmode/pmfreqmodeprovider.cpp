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
#include "pmfreqmodeprovider.h"

#include "../overclock/pmoverclockprovider.h"
#include "core/info/igpuinfo.h"
#include "pmfreqmode.h"
#include <iterator>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMFreqModeProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                            ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    std::vector<std::unique_ptr<IControl>> modeControls;

    for (auto &provider : gpuControlProviders()) {
      auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
      modeControls.insert(modeControls.end(),
                          std::make_move_iterator(newControls.begin()),
                          std::make_move_iterator(newControls.end()));
    }
    if (!modeControls.empty())
      controls.emplace_back(
          std::make_unique<PMFreqMode>(std::move(modeControls)));
  }

  return controls;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
AMD::PMFreqModeProvider::gpuControlProviders() const
{
  return providers_();
}

bool AMD::PMFreqModeProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMFreqModeProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool AMD::PMFreqModeProvider::register_()
{
  PMOverclockProvider::registerProvider(
      std::make_unique<AMD::PMFreqModeProvider>());

  return true;
}

bool const AMD::PMFreqModeProvider::registered_ =
    AMD::PMFreqModeProvider::register_();
