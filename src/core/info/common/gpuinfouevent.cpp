// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuinfouevent.h"

#include "../ihwidtranslator.h"
#include "../infoproviderregistry.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "gpuinfoueventdatasource.h"
#include <algorithm>
#include <cctype>
#include <string_view>
#include <utility>

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
