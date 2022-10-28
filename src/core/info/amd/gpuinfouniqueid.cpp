// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2022 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuinfouniqueid.h"

#include "common/fileutils.h"
#include "core/idatasource.h"
#include "core/info/infoproviderregistry.h"
#include <algorithm>
#include <cctype>
#include <utility>

namespace AMD {
class GPUInfoUniqueIDDataSource
: public IDataSource<std::string, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "unique_id";
  }

  bool read(std::string &data, std::filesystem::path const &path) override
  {
    auto const filePath = path / source();
    if (Utils::File::isFilePathValid(filePath)) {
      auto const lines = Utils::File::readFileLines(filePath);
      if (!lines.empty()) {
        data = lines.front();
        return true;
      }
    }

    return false;
  }
};
} // namespace AMD

AMD::GPUInfoUniqueID::GPUInfoUniqueID(
    std::unique_ptr<IDataSource<std::string, std::filesystem::path const>>
        &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
AMD::GPUInfoUniqueID::provideInfo(Vendor vendor, int, IGPUInfo::Path const &path,
                                  IHWIDTranslator const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  if (vendor == Vendor::AMD) {
    std::string data;
    if (dataSource_->read(data, path.sys)) {
      std::transform(data.cbegin(), data.cend(), data.begin(), ::toupper);
      info.emplace_back(IGPUInfo::Keys::uniqueID, std::move(data));
    }
  }

  return info;
}

std::vector<std::string>
AMD::GPUInfoUniqueID::provideCapabilities(Vendor, int, IGPUInfo::Path const &)
{
  return {};
}

bool AMD::GPUInfoUniqueID::registered_ =
    InfoProviderRegistry::add(std::make_unique<AMD::GPUInfoUniqueID>(
        std::make_unique<GPUInfoUniqueIDDataSource>()));
