// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpuinfolscpu.h"

#include "../infoproviderregistry.h"
#include "common/stringutils.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QProcess>
#include <QStringList>
#include <algorithm>

class CPUInfoLsCpuDataSource : public IDataSource<std::vector<std::string>>
{
 public:
  std::string source() const override
  {
    return "lscpu";
  }

  bool read(std::vector<std::string> &data) override
  {
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");

    QProcess cmd;
    cmd.setProcessChannelMode(QProcess::MergedChannels);
    cmd.setProcessEnvironment(env);
    cmd.start(source().c_str(), QStringList());

    if (cmd.waitForFinished()) {
      auto rawData = cmd.readAllStandardOutput().toStdString();
      data = Utils::String::split(rawData, '\n');
      return true;
    }

    LOG(WARNING) << "lscpu command failed";
    return false;
  }
};

CPUInfoLsCpu::CPUInfoLsCpu(
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
CPUInfoLsCpu::provideInfo(int, std::vector<ICPUInfo::ExecutionUnit> const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  std::vector<std::string> data;
  if (dataSource_->read(data)) {
    addInfo("Architecture", Keys::arch, info, data);
    addInfo("CPU op-mode(s)", Keys::opMode, info, data);
    addInfo("Byte Order", Keys::byteOrder, info, data);
    addInfo("Virtualization", Keys::virt, info, data);
    addInfo("L1d cache", Keys::l1dCache, info, data);
    addInfo("L1i cache", Keys::l1iCache, info, data);
    addInfo("L2 cache", Keys::l2Cache, info, data);
  }

  return info;
}

std::vector<std::string>
CPUInfoLsCpu::provideCapabilities(int,
                                  std::vector<ICPUInfo::ExecutionUnit> const &)
{
  return {};
}

std::string CPUInfoLsCpu::extractLineData(std::string const &line) const
{
  auto colonPos = line.find(':');
  if (colonPos != std::string::npos) {
    auto dataPos = line.find_first_not_of("\t: ", colonPos);
    if (dataPos != std::string::npos)
      return line.substr(dataPos);
  }

  return {};
}

void CPUInfoLsCpu::addInfo(std::string_view target, std::string_view key,
                           std::vector<std::pair<std::string, std::string>> &info,
                           std::vector<std::string> const &data) const
{
  auto keyIt = std::find_if(data.cbegin(), data.cend(),
                            [=](std::string const &line) {
                              return line.find(target) != std::string::npos;
                            });
  if (keyIt != data.cend())
    info.emplace_back(key, extractLineData(*keyIt));
}

bool CPUInfoLsCpu::registered_ = InfoProviderRegistry::add(
    std::make_unique<CPUInfoLsCpu>(std::make_unique<CPUInfoLsCpuDataSource>()));
