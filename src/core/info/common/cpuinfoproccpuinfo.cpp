// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpuinfoproccpuinfo.h"

#include "../ihwidtranslator.h"
#include "../infoproviderregistry.h"
#include "common/fileutils.h"
#include "core/components/cpuutils.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"

class CPUInfoProcCpuInfoDataSource : public IDataSource<std::vector<std::string>>
{
 public:
  std::string source() const override
  {
    return "/proc/cpuinfo";
  }

  bool read(std::vector<std::string> &data) override
  {
    auto lines = Utils::File::readFileLines(source());
    if (!lines.empty()) {
      std::swap(data, lines);
      return true;
    }

    LOG(WARNING) << fmt::format("Cannot retrieve device information from {}",
                                source().c_str());
    return false;
  }
};

CPUInfoProcCpuInfo::CPUInfoProcCpuInfo(
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>> CPUInfoProcCpuInfo::provideInfo(
    int, std::vector<ICPUInfo::ExecutionUnit> const &executionUnits)
{
  std::vector<std::pair<std::string, std::string>> info;

  std::vector<std::string> data;
  if (!executionUnits.empty() && dataSource_->read(data)) {

    info.emplace_back(ICPUInfo::Keys::executionUnits,
                      std::to_string(executionUnits.size()));
    auto cpuId = executionUnits.front().cpuId;

    addInfo("vendor_id", ICPUInfo::Keys::vendorId, cpuId, info, data);
    addInfo("cpu family", ICPUInfo::Keys::cpuFamily, cpuId, info, data);
    addInfo("model", ICPUInfo::Keys::model, cpuId, info, data);
    addInfo("model name", ICPUInfo::Keys::modelName, cpuId, info, data);
    addInfo("stepping", ICPUInfo::Keys::stepping, cpuId, info, data);
    addInfo("microcode", ICPUInfo::Keys::ucode, cpuId, info, data);
    addInfo("cache size", ICPUInfo::Keys::l3Cache, cpuId, info, data);
    addInfo("cpu cores", ICPUInfo::Keys::cores, cpuId, info, data);
    addInfo("flags", ICPUInfo::Keys::flags, cpuId, info, data);
    addInfo("bugs", ICPUInfo::Keys::bugs, cpuId, info, data);
    addInfo("bogomips", ICPUInfo::Keys::bogomips, cpuId, info, data);
  }

  return info;
}

std::vector<std::string> CPUInfoProcCpuInfo::provideCapabilities(
    int, std::vector<ICPUInfo::ExecutionUnit> const &)
{
  return {};
}

void CPUInfoProcCpuInfo::addInfo(
    std::string_view target, std::string_view key, int cpuId,
    std::vector<std::pair<std::string, std::string>> &info,
    std::vector<std::string> const &lines) const
{
  auto data = Utils::CPU::parseProcCpuInfo(lines, cpuId, target);
  if (data.has_value())
    info.emplace_back(key, *data);
}

bool CPUInfoProcCpuInfo::registered_ =
    InfoProviderRegistry::add(std::make_unique<CPUInfoProcCpuInfo>(
        std::make_unique<CPUInfoProcCpuInfoDataSource>()));
