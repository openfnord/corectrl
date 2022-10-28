// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fancurveprovider.h"

#include "../fanmodeprovider.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/info/igpuinfo.h"
#include "core/info/iswinfo.h"
#include "core/sysfsdatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fancurve.h"
#include "fmt/format.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>

std::vector<std::unique_ptr<IControl>>
AMD::FanCurveProvider::provideGPUControls(IGPUInfo const &gpuInfo,
                                          ISWInfo const &swInfo) const
{
  std::vector<std::unique_ptr<IControl>> controls;

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
        auto pwm = path.value() / "pwm1";
        auto tempInput = path.value() / "temp1_input";
        auto tempCrit = path.value() / "temp1_crit";
        if (Utils::File::isSysFSEntryValid(pwm) &&
            Utils::File::isSysFSEntryValid(pwmEnable) &&
            Utils::File::isSysFSEntryValid(tempInput) &&
            Utils::File::isSysFSEntryValid(tempCrit)) {

          int tempCritValue{0};
          Utils::String::toNumber(tempCritValue,
                                  Utils::File::readFileLines(tempCrit).front());
          tempCritValue = (tempCritValue > 0 &&
                           tempCritValue < 150000) // check bogus values, see #103
                              ? tempCritValue / 1000
                              : 90;

          unsigned int value;

          auto pwmEnableLines = Utils::File::readFileLines(pwmEnable);
          auto pwmEnableValid = Utils::String::toNumber<unsigned int>(
              value, pwmEnableLines.front());

          auto pwmLines = Utils::File::readFileLines(pwm);
          auto pwmValid = Utils::String::toNumber<unsigned int>(
              value, pwmLines.front());

          int tempInputValue;
          auto tempInputLines = Utils::File::readFileLines(tempInput);
          auto tempInputValid = Utils::String::toNumber<int>(
              tempInputValue, tempInputLines.front());

          if (pwmEnableValid && pwmValid && tempInputValid) {

            controls.emplace_back(std::make_unique<AMD::FanCurve>(
                std::make_unique<SysFSDataSource<unsigned int>>(
                    pwmEnable,
                    [](std::string const &data, unsigned int &output) {
                      Utils::String::toNumber<unsigned int>(output, data);
                    }),
                std::make_unique<SysFSDataSource<unsigned int>>(
                    pwm,
                    [](std::string const &data, unsigned int &output) {
                      Utils::String::toNumber<unsigned int>(output, data);
                    }),
                std::make_unique<SysFSDataSource<int>>(
                    tempInput,
                    [](std::string const &data, int &output) {
                      int value;
                      Utils::String::toNumber<int>(value, data);
                      output = value / 1000;
                    }),
                units::temperature::celsius_t(0),
                units::temperature::celsius_t(tempCritValue)));
          }
          else {
            if (!pwmEnableValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          pwmEnable.string());
              LOG(ERROR) << pwmEnableLines.front().c_str();
            }

            if (!pwmValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          pwm.string());
              LOG(ERROR) << pwmLines.front().c_str();
            }

            if (!tempInputValid) {
              LOG(WARNING) << fmt::format("Unknown data format on {}",
                                          tempInput.string());
              LOG(ERROR) << tempInputLines.front().c_str();
            }
          }
        }
      }
    }
  }

  return controls;
}

bool const AMD::FanCurveProvider::registered_ =
    AMD::FanModeProvider::registerProvider(
        std::make_unique<AMD::FanCurveProvider>());
