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
#include "gpuinfouevent.h"

#include "../ihwidtranslator.h"
#include "../infoproviderregistry.h"
#include "common/fileutils.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <algorithm>
#include <cctype>
#include <string_view>
#include <utility>

class GPUInfoUeventDataSource
: public IDataSource<std::vector<std::string>, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "uevent";
  }

  bool read(std::vector<std::string> &data,
            std::filesystem::path const &path) override
  {
    auto const filePath = path / source();
    auto lines = Utils::File::readFileLines(filePath);
    if (!lines.empty()) {
      std::swap(data, lines);
      return true;
    }

    LOG(WARNING) << fmt::format("Cannot retrieve device information from {}",
                                filePath.c_str());
    return false;
  }
};

GPUInfoUevent::GPUInfoUevent(
    std::unique_ptr<IDataSource<std::vector<std::string>, std::filesystem::path const>>
        &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
GPUInfoUevent::provideInfo(Vendor, int, IGPUInfo::Path const &path,
                           IHWIDTranslator const &hwIDTranslator)
{
  std::vector<std::pair<std::string, std::string>> info;

  static constexpr std::string_view driverStr{"DRIVER"};
  static constexpr std::string_view pciIdStr{"PCI_ID"};
  static constexpr std::string_view pciSubsysIdStr{"PCI_SUBSYS_ID"};
  static constexpr std::string_view pciSlotStr{"PCI_SLOT_NAME"};

  std::vector<std::string> data;
  if (dataSource_->read(data, path.sys)) {
    std::string vendorId;
    std::string deviceId;
    std::string subvendorId;
    std::string subdeviceId;
    std::string pciSlot;
    std::string driver;

    for (auto &line : data) {
      if (line.find(driverStr) == 0) {
        driver = line.substr(driverStr.length() + 1, std::string::npos);
      }
      else if (line.find(pciIdStr) == 0) {
        auto colonPos = line.find(':', pciIdStr.length() + 1);
        vendorId = line.substr(pciIdStr.length() + 1,
                               colonPos - pciIdStr.length() - 1);
        deviceId = line.substr(colonPos + 1, std::string::npos);
      }
      else if (line.find(pciSubsysIdStr) == 0) {
        auto colonPos = line.find(':', pciSubsysIdStr.length() + 1);
        subvendorId = line.substr(pciSubsysIdStr.length() + 1,
                                  colonPos - pciSubsysIdStr.length() - 1);
        subdeviceId = line.substr(colonPos + 1, std::string::npos);
      }
      else if (line.find(pciSlotStr) == 0) {
        pciSlot = line.substr(pciSlotStr.length() + 1, std::string::npos);
      }
    }

    if (vendorId.empty())
      LOG(ERROR) << "Cannot retrieve vendor ID";
    if (deviceId.empty())
      LOG(ERROR) << "Cannot retrieve device ID";
    if (subvendorId.empty())
      LOG(ERROR) << "Cannot retrieve subvendor ID";
    if (subdeviceId.empty())
      LOG(ERROR) << "Cannot retrieve subdevice ID";
    if (pciSlot.empty())
      LOG(ERROR) << "Cannot retrieve pci slot";
    if (driver.empty())
      LOG(ERROR) << "Cannot retrieve driver";

    // ensure that all ids are in uppercase
    std::transform(vendorId.cbegin(), vendorId.cend(), vendorId.begin(),
                   ::toupper);
    std::transform(deviceId.cbegin(), deviceId.cend(), deviceId.begin(),
                   ::toupper);
    std::transform(subvendorId.cbegin(), subvendorId.cend(),
                   subvendorId.begin(), ::toupper);
    std::transform(subdeviceId.cbegin(), subdeviceId.cend(),
                   subdeviceId.begin(), ::toupper);

    // populate info
    auto vendorName = hwIDTranslator.vendor(vendorId);
    if (!vendorName.empty())
      info.emplace_back(IGPUInfo::Keys::vendorName, std::move(vendorName));

    auto deviceName = hwIDTranslator.device(vendorId, deviceId);
    if (!deviceName.empty())
      info.emplace_back(IGPUInfo::Keys::deviceName, std::move(deviceName));

    auto subdeviceName = hwIDTranslator.subdevice(vendorId, deviceId,
                                                  subvendorId, subdeviceId);
    if (!subdeviceName.empty())
      info.emplace_back(IGPUInfo::Keys::subdeviceName, std::move(subdeviceName));

    info.emplace_back(IGPUInfo::Keys::driver, std::move(driver));
    info.emplace_back(IGPUInfo::Keys::pciSlot, std::move(pciSlot));
    info.emplace_back(IGPUInfo::Keys::vendorID, std::move(vendorId));
    info.emplace_back(IGPUInfo::Keys::deviceID, std::move(deviceId));
    info.emplace_back(IGPUInfo::Keys::subvendorID, std::move(subvendorId));
    info.emplace_back(IGPUInfo::Keys::subdeviceID, std::move(subdeviceId));
  }

  return info;
}

std::vector<std::string>
GPUInfoUevent::provideCapabilities(Vendor, int, IGPUInfo::Path const &)
{
  return {};
}

bool GPUInfoUevent::registered_ = InfoProviderRegistry::add(
    std::make_unique<GPUInfoUevent>(std::make_unique<GPUInfoUeventDataSource>()));
