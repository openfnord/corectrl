// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

std::vector<std::unique_ptr<IControl>>
AMD::FanAutoProvider::provideGPUControls(IGPUInfo const &gpuInfo,
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
        if (Utils::File::isSysFSEntryValid(pwmEnable)) {

          controls.emplace_back(std::make_unique<AMD::FanAuto>(
              std::make_unique<SysFSDataSource<unsigned int>>(
                  pwmEnable, [](std::string const &data, unsigned int &output) {
                    Utils::String::toNumber<unsigned int>(output, data);
                  })));
        }
      }
    }
  }

  return controls;
}

bool const AMD::FanAutoProvider::registered_ =
    AMD::FanModeProvider::registerProvider(
        std::make_unique<AMD::FanAutoProvider>());
