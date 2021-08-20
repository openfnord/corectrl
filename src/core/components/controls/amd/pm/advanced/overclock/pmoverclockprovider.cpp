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
#include "pmoverclockprovider.h"

#include "../pmadvancedprovider.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "pmoverclock.h"
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

std::vector<std::unique_ptr<IControl>>
AMD::PMOverclockProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                             ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && (kernel >= std::make_tuple(4, 8, 0) &&
                               kernel < std::make_tuple(4, 17, 0))) {

      std::vector<std::unique_ptr<IControl>> modeControls;

      for (auto &provider : providers_()) {
        auto newControls = provider->provideGPUControls(gpuInfo, swInfo);
        modeControls.insert(modeControls.end(),
                            std::make_move_iterator(newControls.begin()),
                            std::make_move_iterator(newControls.end()));
      }
      if (!modeControls.empty())
        controls.emplace_back(
            std::make_unique<PMOverclock>(std::move(modeControls)));
    }
  }

  return controls;
}

bool AMD::PMOverclockProvider::registerProvider(
    std::unique_ptr<IGPUControlProvider::IProvider> &&provider)
{
  providers_().emplace_back(std::move(provider));
  return true;
}

std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> &
AMD::PMOverclockProvider::providers_()
{
  static std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> providers;
  return providers;
}

bool const AMD::PMOverclockProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMOverclockProvider>());
