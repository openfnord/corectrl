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
#include "gpuinfovulkan.h"

#include "../infoproviderregistry.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QProcess>
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
    cmd.start(source().c_str());

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
