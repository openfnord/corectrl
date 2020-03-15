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
