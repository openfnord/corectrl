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
#include "pmadvancedprovider.h"

#include "../pmperfmodeprovider.h"
#include "core/info/igpuinfo.h"
#include "pmadvanced.h"
#include <iterator>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMAdvancedProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                            ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    std::vector<std::unique_ptr<IControl>> groupControls;

    for (auto &provider : providers_()) {
      auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
      groupControls.insert(groupControls.end(),
                           std::make_move_iterator(newControls.begin()),
                           std::make_move_iterator(newControls.end()));
    }
    if (!groupControls.empty())
      controls.emplace_back(
          std::make_unique<PMAdvanced>(std::move(groupControls)));
  }

  return controls;
}

bool AMD::PMAdvancedProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMAdvancedProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMAdvancedProvider::registered_ =
    AMD::PMPerfModeProvider::registerProvider(
        std::make_unique<AMD::PMAdvancedProvider>());
