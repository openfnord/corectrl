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
#include "fanautoprovider.h"

#include "../fanmodeprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "fanauto.h"
#include <filesystem>
#include <memory>
#include <tuple>

namespace fs = std::filesystem;

std::unique_ptr<IControl>
AMD::FanAutoProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                        ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if ((driver == "radeon" && kernel >= std::make_tuple(4, 0, 0)) ||
        (driver == "amdgpu" && kernel >= std::make_tuple(4, 2, 0))) {

      auto path =
          Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
      if (path.has_value()) {

        auto pwmEnable = path.value() / "pwm1_enable";
        if (Utils::File::isSysFSEntryValid(pwmEnable)) {

          return std::make_unique<AMD::FanAuto>(
              std::make_unique<SysFSDataSource<unsigned int>>(
                  pwmEnable, [](std::string const &data, unsigned int &output) {
                    Utils::String::toNumber<unsigned int>(output, data);
                  }));
        }
      }
    }
  }

  return nullptr;
}

bool const AMD::FanAutoProvider::registered_ =
    AMD::FanModeProvider::registerProvider(
        std::make_unique<AMD::FanAutoProvider>());
