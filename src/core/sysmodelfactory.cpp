// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "sysmodelfactory.h"

#include "common/fileutils.h"
#include "common/stringutils.h"
#include "components/controls/icontrol.h"
#include "components/controls/icpucontrolprovider.h"
#include "components/controls/igpucontrolprovider.h"
#include "components/cpu.h"
#include "components/cpuutils.h"
#include "components/gpu.h"
#include "components/sensors/icpusensorprovider.h"
#include "components/sensors/igpusensorprovider.h"
#include "components/sensors/isensor.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "info/cpuinfo.h"
#include "info/gpuinfo.h"
#include "info/ihwidtranslator.h"
#include "info/iswinfo.h"
#include "isyscomponent.h"
#include "isysexplorer.h"
#include "sysmodel.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

SysModelFactory::SysModelFactory(
    std::unique_ptr<ISWInfo> &&swInfo,
    std::unique_ptr<ISysExplorer> &&sysExplorer,
    std::unique_ptr<IHWIDTranslator> &&hwIDTranslator,
    std::unique_ptr<ICPUControlProvider> &&cpuControlProvider,
    std::unique_ptr<ICPUSensorProvider> &&cpuSensorProvider,
    std::unique_ptr<IGPUControlProvider> &&gpuControlProvider,
    std::unique_ptr<IGPUSensorProvider> &&gpuSensorProvider,
    ICPUInfo::IProviderRegistry const &cpuInfoProviderRegistry,
    IGPUInfo::IProviderRegistry const &gpuInfoProviderRegistry) noexcept
: swInfo_(std::move(swInfo))
, sysExplorer_(std::move(sysExplorer))
, hwidTranslator_(std::move(hwIDTranslator))
, cpuControlProvider_(std::move(cpuControlProvider))
, cpuSensorProvider_(std::move(cpuSensorProvider))
, gpuControlProvider_(std::move(gpuControlProvider))
, gpuSensorProvider_(std::move(gpuSensorProvider))
, cpuInfoProviderRegistry_(cpuInfoProviderRegistry)
, gpuInfoProviderRegistry_(gpuInfoProviderRegistry)
{
}

std::unique_ptr<ISysModel> SysModelFactory::build() const
{
  std::vector<std::unique_ptr<ISysComponent>> components;

  auto gpuInfo = createGPUInfo();
  for (auto &info : gpuInfo)
    components.emplace_back(createGPU(std::move(info), *swInfo_));

  auto cpuInfo = createCPUInfo();
  for (auto &info : cpuInfo)
    components.emplace_back(createCPU(std::move(info), *swInfo_));

  return std::make_unique<SysModel>(swInfo_, std::move(components));
}

std::vector<std::unique_ptr<ICPUInfo>> SysModelFactory::createCPUInfo() const
{
  auto cpuInfo = parseCPUInfo();
  for (auto &info : cpuInfo)
    info->initialize(cpuInfoProviderRegistry_.cpuInfoProviders());

  return cpuInfo;
}

std::unique_ptr<ICPU>
SysModelFactory::createCPU(std::unique_ptr<ICPUInfo> &&cpuInfo,
                           ISWInfo const &swInfo) const
{
  // create CPU controls
  std::vector<std::unique_ptr<IControl>> controls;
  auto &cpuControlProviders = cpuControlProvider_->cpuControlProviders();
  for (auto &provider : cpuControlProviders) {
    auto newControls = provider->provideCPUControls(*cpuInfo, swInfo);
    controls.insert(controls.end(), std::make_move_iterator(newControls.begin()),
                    std::make_move_iterator(newControls.end()));
  }

  // create CPU sensors
  std::vector<std::unique_ptr<ISensor>> sensors;
  auto &cpuSensorProviders = cpuSensorProvider_->cpuSensorProviders();
  for (auto &provider : cpuSensorProviders) {
    auto newSensors = provider->provideCPUSensors(*cpuInfo, swInfo);
    sensors.insert(sensors.end(), std::make_move_iterator(newSensors.begin()),
                   std::make_move_iterator(newSensors.end()));
  }

  return std::make_unique<CPU>(std::move(cpuInfo), std::move(controls),
                               std::move(sensors));
}

std::vector<std::unique_ptr<IGPUInfo>> SysModelFactory::createGPUInfo() const
{
  namespace fs = std::filesystem;
  std::vector<std::unique_ptr<IGPUInfo>> gpuInfo;

  fs::path sysBasePath{"/sys/class/drm"};
  auto deviceNames = sysExplorer_->renderers();
  for (auto &deviceName : deviceNames) {

    auto sysPath = sysBasePath / deviceName / "device";
    auto devPath = fs::path("/dev/dri").append(deviceName);
    auto vendor = parseVendor(sysBasePath / deviceName / "device/vendor");
    int const gpuIndex = computeGPUIndex(deviceName);

    auto info = std::make_unique<GPUInfo>(vendor, gpuIndex,
                                          GPUInfo::Path(sysPath, devPath));
    info->initialize(gpuInfoProviderRegistry_.gpuInfoProviders(),
                     *hwidTranslator_);
    gpuInfo.emplace_back(std::move(info));
  }

  return gpuInfo;
}

std::unique_ptr<IGPU>
SysModelFactory::createGPU(std::unique_ptr<IGPUInfo> &&gpuInfo,
                           ISWInfo const &swInfo) const
{
  // create GPU controls
  std::vector<std::unique_ptr<IControl>> controls;
  auto &gpuControlProviders = gpuControlProvider_->gpuControlProviders();
  for (auto &provider : gpuControlProviders) {
    auto newControls = provider->provideGPUControls(*gpuInfo, swInfo);
    controls.insert(controls.end(), std::make_move_iterator(newControls.begin()),
                    std::make_move_iterator(newControls.end()));
  }

  // create GPU sensors
  std::vector<std::unique_ptr<ISensor>> sensors;
  auto &gpuSensorProviders = gpuSensorProvider_->gpuSensorProviders();
  for (auto &provider : gpuSensorProviders) {
    auto newSensors = provider->provideGPUSensors(*gpuInfo, swInfo);
    sensors.insert(sensors.end(), std::make_move_iterator(newSensors.begin()),
                   std::make_move_iterator(newSensors.end()));
  }

  return std::make_unique<GPU>(std::move(gpuInfo), std::move(controls),
                               std::move(sensors));
}

std::vector<std::unique_ptr<ICPUInfo>> SysModelFactory::parseCPUInfo() const
{
  static constexpr std::string_view cpuIdStr{"processor"};

  std::filesystem::path const basePath{"/sys/devices/system/cpu"};
  std::vector<std::unique_ptr<ICPUInfo>> cpuInfo;

  int value;
  std::optional<int> cpuId;

  auto cpuInfoLines = Utils::File::readFileLines("/proc/cpuinfo");
  for (auto &line : cpuInfoLines) {

    if (line.empty()) { // push collected info
      std::optional<int> socketId;
      std::optional<int> coreId;

      auto socketIdStr = Utils::CPU::parseProcCpuInfo(cpuInfoLines, *cpuId,
                                                      "physical id");
      if (socketIdStr.has_value() &&
          Utils::String::toNumber<int>(value, *socketIdStr))
        socketId = value;

      auto coreIdStr = Utils::CPU::parseProcCpuInfo(cpuInfoLines, *cpuId,
                                                    "core id");
      if (coreIdStr.has_value() &&
          Utils::String::toNumber<int>(value, *coreIdStr))
        coreId = value;

      if (cpuId.has_value() && socketId.has_value() && coreId.has_value()) {
        std::string cpuDir{"cpu"};
        cpuDir.append(std::to_string(*cpuId));
        auto executionUnitPath = basePath / cpuDir;

        auto infoIt = std::find_if(
            cpuInfo.cbegin(), cpuInfo.cend(),
            [=](auto &info) { return info->socketId() == *socketId; });
        if (infoIt != cpuInfo.cend())
          static_cast<CPUInfo *>(infoIt->get())
              ->addExecutionUnit(
                  ICPUInfo::ExecutionUnit(*cpuId, *coreId, executionUnitPath));
        else {
          std::vector<ICPUInfo::ExecutionUnit> units{
              ICPUInfo::ExecutionUnit(*cpuId, *coreId, executionUnitPath)};

          cpuInfo.emplace_back(
              std::make_unique<CPUInfo>(*socketId, std::move(units)));
        }

        socketId = cpuId = coreId = std::nullopt;
      }
      else {
        LOG(ERROR) << "Cannot parse some data from /proc/cpuinfo";
        return {};
      }
    }

    auto cpuIdIt = line.find(cpuIdStr);
    if (cpuIdIt != std::string::npos) {
      auto indexPos = line.find_first_not_of("\t: ", cpuIdStr.size());
      if (indexPos != std::string::npos &&
          Utils::String::toNumber<int>(value, line.substr(indexPos))) {
        cpuId = value;
        continue;
      }
    }
  }

  return cpuInfo;
}

int SysModelFactory::computeGPUIndex(std::string const &deviceRenderDName) const
{
  auto const indexStr = Utils::String::cleanPrefix(deviceRenderDName,
                                                   "renderD");
  int index = -1;
  if (Utils::String::toNumber<int>(index, indexStr))
    index -= 128;
  else
    LOG(ERROR) << fmt::format("Cannot compute GPU index for device {}.",
                              deviceRenderDName.c_str());
  return index;
}

Vendor SysModelFactory::parseVendor(std::filesystem::path const &vendorPath) const
{
  Vendor vendor{-1};

  auto const lines = Utils::File::readFileLines(vendorPath);
  if (!lines.empty()) {
    int dataValue;
    if (Utils::String::toNumber<int>(dataValue, lines.front(), 16))
      vendor = Vendor{dataValue};
    else
      LOG(ERROR) << fmt::format("Cannot parse vendor id from file {}.",
                                vendorPath.c_str());
  }

  return vendor;
}
