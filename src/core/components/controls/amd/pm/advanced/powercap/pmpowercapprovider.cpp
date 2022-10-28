// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowercapprovider.h"

#include "../pmadvancedprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "pmpowercap.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

std::vector<std::unique_ptr<IControl>>
AMD::PMPowerCapProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                            ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

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

          auto power1CapLines = Utils::File::readFileLines(power1CapPath);
          unsigned long power1CapValue;
          auto valueValid = Utils::String::toNumber<unsigned long>(
              power1CapValue, power1CapLines.front());

          auto power1CapMinLines = Utils::File::readFileLines(power1CapMinPath);
          unsigned long power1CapMinValue;
          auto minValueValid = Utils::String::toNumber<unsigned long>(
              power1CapMinValue, power1CapMinLines.front());

          auto power1CapMaxLines = Utils::File::readFileLines(power1CapMaxPath);
          unsigned long power1CapMaxValue;
          auto maxValueValid = Utils::String::toNumber<unsigned long>(
              power1CapMaxValue, power1CapMaxLines.front());

          if (valueValid && minValueValid && maxValueValid) {

            controls.emplace_back(std::make_unique<AMD::PMPowerCap>(
                std::make_unique<SysFSDataSource<unsigned long>>(
                    power1CapPath,
                    [](std::string const &data, unsigned long &output) {
                      Utils::String::toNumber<unsigned long>(output, data);
                    }),
                units::power::microwatt_t(power1CapMinValue),
                units::power::microwatt_t(power1CapMaxValue)));
          }
          else {
            if (!valueValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          power1CapPath.string());
              LOG(ERROR) << power1CapLines.front();
            }

            if (!minValueValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          power1CapMinPath.string());
              LOG(ERROR) << power1CapMinLines.front();
            }

            if (!maxValueValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          power1CapMaxPath.string());
              LOG(ERROR) << power1CapMaxLines.front();
            }
          }
        }
      }
    }
  }

  return controls;
}

bool const AMD::PMPowerCapProvider::registered_ =
    AMD::PMAdvancedProvider::registerProvider(
        std::make_unique<AMD::PMPowerCapProvider>());
