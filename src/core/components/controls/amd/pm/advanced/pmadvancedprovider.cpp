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
#include <utility>

std::unique_ptr<IControl>
AMD::PMAdvancedProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                           ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    std::vector<std::unique_ptr<IControl>> controls;

    for (auto &provider : providers_()) {
      auto control = provider->provideGPUControl(gpuInfo, swInfo);
      if (control != nullptr)
        controls.emplace_back(std::move(control));
    }
    if (!controls.empty())
      return std::make_unique<PMAdvanced>(std::move(controls));
  }

  return nullptr;
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
