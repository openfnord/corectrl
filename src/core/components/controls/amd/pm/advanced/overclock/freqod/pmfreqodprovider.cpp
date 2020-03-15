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
#include "pmfreqodprovider.h"

#include "../pmoverclockprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/components/amdutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmfreqod.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

std::unique_ptr<IControl>
AMD::PMFreqOdProvider::provideGPUControl(IGPUInfo const &gpuInfo,
                                         ISWInfo const &swInfo) const
{
  if (gpuInfo.vendor() == Vendor::AMD) {
    auto kernel =
        Utils::String::parseVersion(swInfo.info(ISWInfo::Keys::kernelVersion));
    auto driver = gpuInfo.info(IGPUInfo::Keys::driver);

    if (driver == "amdgpu" && (kernel >= std::make_tuple(4, 8, 0) &&
                               kernel < std::make_tuple(4, 17, 0))) {

      auto sclkOd = gpuInfo.path().sys / "pp_sclk_od";
      auto mclkOd = gpuInfo.path().sys / "pp_mclk_od";
      auto dpmSclk = gpuInfo.path().sys / "pp_dpm_sclk";
      auto dpmMclk = gpuInfo.path().sys / "pp_dpm_mclk";
      if (Utils::File::isSysFSEntryValid(sclkOd) &&
          Utils::File::isSysFSEntryValid(mclkOd) &&
          Utils::File::isSysFSEntryValid(dpmSclk) &&
          Utils::File::isSysFSEntryValid(dpmMclk)) {

        unsigned int odValue;

        auto sclkOdLines = Utils::File::readFileLines(sclkOd);
        auto sclkOdValid = Utils::String::toNumber<unsigned int>(
            odValue, sclkOdLines.front());

        auto mclkOdLines = Utils::File::readFileLines(mclkOd);
        auto mclkOdValid = Utils::String::toNumber<unsigned int>(
            odValue, mclkOdLines.front());

        auto dpmSclkLines = Utils::File::readFileLines(dpmSclk);
        auto sclkStates = Utils::AMD::parseDPMStates(dpmSclkLines);

        auto dpmMclkLines = Utils::File::readFileLines(dpmMclk);
        auto mclkStates = Utils::AMD::parseDPMStates(dpmMclkLines);

        if (sclkOdValid && mclkOdValid && sclkStates.has_value() &&
            mclkStates.has_value()) {

          return std::make_unique<AMD::PMFreqOd>(
              std::make_unique<SysFSDataSource<unsigned int>>(
                  sclkOd,
                  [](std::string const &data, unsigned int &output) {
                    Utils::String::toNumber<unsigned int>(output, data);
                  }),
              std::make_unique<SysFSDataSource<unsigned int>>(
                  mclkOd,
                  [](std::string const &data, unsigned int &output) {
                    Utils::String::toNumber<unsigned int>(output, data);
                  }),
              sclkStates.value(), mclkStates.value());
        }
        else {
          if (!sclkOdValid) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        sclkOd.string());
            LOG(ERROR) << sclkOdLines.front().c_str();
          }

          if (!mclkOdValid) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        mclkOd.string());
            LOG(ERROR) << mclkOdLines.front().c_str();
          }

          if (!sclkStates.has_value()) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        dpmSclk.string());
            for (auto &line : dpmSclkLines)
              LOG(ERROR) << line.c_str();
          }

          if (!mclkStates.has_value()) {
            LOG(WARNING) << fmt::format("Unknown data format on {}",
                                        dpmMclk.string());
            for (auto &line : dpmMclkLines)
              LOG(ERROR) << line.c_str();
          }
        }
      }
    }
  }

  return nullptr;
}

bool const AMD::PMFreqOdProvider::registered_ =
    AMD::PMOverclockProvider::registerProvider(
        std::make_unique<AMD::PMFreqOdProvider>());
