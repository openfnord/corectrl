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
#include "pmpowercapprovider.h"

#include "../pmadvancedprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"

#include "pmpowercap.h"

namespace fs = std::filesystem;

std::unique_ptr<IControl>
AMD::PMPowerCapProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                           ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && kernel >= std::make_tuple(4, 17, 0)) {

      auto path =
          Utils::File::findHWMonXDirectory(gpuInfo.path().sys / "hwmon");
      if (path.has_value()) {

        auto power1CapPath = path.value() / "power1_cap";
        auto power1CapMinPath = path.value() / "power1_cap_min";
        auto power1CapMaxPath = path.value() / "power1_cap_max";
        if (Utils::File::isSysFSEntryValid(power1CapPath) &&
            Utils::File::isSysFSEntryValid(power1CapMinPath) &&
            Utils::File::isSysFSEntryValid(power1CapMaxPath)) {

          auto power1CapMinLines = Utils::File::readFileLines(power1CapMinPath);
          auto power1CapMaxLines = Utils::File::readFileLines(power1CapMaxPath);
          unsigned long power1CapMinValue;
          unsigned long power1CapMaxValue;
          if (Utils::String::toNumber<unsigned long>(
                  power1CapMinValue, power1CapMinLines.front()) &&
              Utils::String::toNumber<unsigned long>(
                  power1CapMaxValue, power1CapMaxLines.front())) {

            return std::make_unique<AMD::PMPowerCap>(
                std::make_unique<SysFSDataSource<unsigned long>>(
                    power1CapPath,
                    [](std::string const &data, unsigned long &output) {
                      Utils::String::toNumber<unsigned long>(output, data);
                    }),
                units::power::microwatt_t(power1CapMinValue),
                units::power::microwatt_t(power1CapMaxValue));
          }
        }
      }
    }
  }

  return nullptr;
}

bool const AMD::PMPowerCapProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMPowerCapProvider>());
