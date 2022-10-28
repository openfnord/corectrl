// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuinfovulkan.h"

#include "../infoproviderregistry.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QProcess>
#include <QStringList>
#include <utility>

class GPUInfoVulkanDataSource : public IDataSource<std::string>
{
 public:
  std::string source() const override
  {
    return "vulkaninfo";
  }

  bool read(std::string &data) override
  {
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");

    QProcess cmd;
    cmd.setProcessChannelMode(QProcess::MergedChannels);
    cmd.setProcessEnvironment(env);
    cmd.start(source().c_str(), QStringList());

    if (cmd.waitForFinished()) {
      data = cmd.readAllStandardOutput().toStdString();
      return true;
    }

    LOG(WARNING) << "vulkaninfo command failed";
    return false;
  }
};

GPUInfoVulkan::GPUInfoVulkan(
    std::unique_ptr<IDataSource<std::string>> &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
GPUInfoVulkan::provideInfo(Vendor, int gpuIndex, IGPUInfo::Path const &,
                           IHWIDTranslator const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  static constexpr std::string_view devicePropStr("VkPhysicalDeviceProperties");

  std::string data;
  if (dataSource_->read(data)) {
    int currentGPUIndex = 0;
    auto devicePropStrPos = data.find(devicePropStr);
    while (devicePropStrPos != std::string::npos) {

      if (currentGPUIndex == gpuIndex) {
        auto apiVersion = parseApiVersion(data, devicePropStrPos);
        if (!apiVersion.empty())
          info.emplace_back(GPUInfoVulkan::Keys::apiVersion,
                            std::move(apiVersion));
        break;
      }

      devicePropStrPos = data.find(devicePropStr,
                                   devicePropStrPos + devicePropStr.length());
      currentGPUIndex++;
    }
  }

  return info;
}

std::vector<std::string>
GPUInfoVulkan::provideCapabilities(Vendor, int, IGPUInfo::Path const &)
{
  return {};
}

std::string GPUInfoVulkan::parseApiVersion(std::string const &src,
                                           size_t pos) const
{
  static constexpr std::string_view apiVerStr("apiVersion");

  auto apiVerPos = src.find(apiVerStr, pos);
  if (apiVerPos != std::string::npos) {

    auto startPos = src.find_first_not_of("= ", apiVerPos + apiVerStr.length());

    // version inside parentheses is preferred
    auto openParenPos = src.find("(", startPos);
    if (openParenPos != std::string::npos) {
      auto endPos = src.find(")", openParenPos);
      auto version = src.substr(openParenPos + 1, endPos - openParenPos - 1);

      return version;
    }

    // fallback version
    auto endPos = src.find("\n", startPos);
    auto version = src.substr(startPos, endPos - startPos);

    return version;
  }
  else
    LOG(ERROR) << fmt::format("Cannot find '{}' in vulkaninfo output",
                              apiVerStr.data());

  return std::string{};
}

bool GPUInfoVulkan::registered_ = InfoProviderRegistry::add(
    std::make_unique<GPUInfoVulkan>(std::make_unique<GPUInfoVulkanDataSource>()));
