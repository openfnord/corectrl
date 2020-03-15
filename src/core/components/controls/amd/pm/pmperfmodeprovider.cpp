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
#include "pmperfmodeprovider.h"

#include "core/components/controls/gpucontrolprovider.h"
#include "core/info/igpuinfo.h"
#include "pmperfmode.h"
#include <utility>

std::unique_ptr<IControl>
AMD::PMPerfModeProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                           ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    std::vector<std::unique_ptr<IControl>> controls;

    for (auto &provider : gpuControlProviders()) {
      auto control = provider->provideGPUControl(gpuInfo, swInfo);
      if (control != nullptr)
        controls.emplace_back(std::move(control));
    }
    if (!controls.empty())
      return std::make_unique<PMPerfMode>(std::move(controls));
  }

  return nullptr;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
AMD::PMPerfModeProvider::gpuControlProviders() const
{
  return providers_();
}

bool AMD::PMPerfModeProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMPerfModeProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMPerfModeProvider::registered_ =
    GPUControlProvider::registerProvider(
        std::make_unique<AMD::PMPerfModeProvider>());
