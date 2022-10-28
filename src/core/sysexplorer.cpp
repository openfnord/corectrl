// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "sysexplorer.h"

#include "common/fileutils.h"
#include "common/stringutils.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <algorithm>
#include <utility>

SysExplorer::SysExplorer(std::vector<Vendor> gpuVendors) noexcept
: gpuVendors_(std::move(gpuVendors))
{
}

std::vector<std::string> SysExplorer::renderers()
{
  namespace fs = std::filesystem;
  std::vector<std::string> devices;

  fs::path renderBasePath{"/sys/class/drm"};
  for (auto &drmEntry : fs::directory_iterator(renderBasePath)) {
    auto &renderDPath = drmEntry.path();

    // use the entries starting with "renderD"
    auto const renderDName = renderDPath.filename().string();
    if (renderDName.find("renderD") == 0) {

      auto sysPath = renderDPath / "device";
      if (Utils::File::isDirectoryPathValid(sysPath)) {

        // skip unsupported devices
        if (!checkGPUVendor(sysPath))
          continue;

        devices.emplace_back(std::move(renderDName));
      }
    }
  }

  return devices;
}

bool SysExplorer::checkGPUVendor(std::filesystem::path sysPath) const
{
  auto const vendorPath = sysPath / "vendor";
  auto lines = Utils::File::readFileLines(vendorPath);
  if (!lines.empty()) {
    int vendor;
    if (Utils::String::toNumber<int>(vendor, lines.front(), 16)) {
      if (std::find(gpuVendors_.cbegin(), gpuVendors_.cend(), Vendor{vendor}) !=
          gpuVendors_.cend())
        return true;
    }
    else
      LOG(ERROR) << fmt::format("Cannot parse vendor id from file {}.",
                                vendorPath.c_str());
  }
  return false;
}
