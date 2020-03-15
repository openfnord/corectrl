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
#include "gpuinfopm.h"

#include "common/fileutils.h"
#include "core/idatasource.h"
#include "core/info/infoproviderregistry.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <utility>

namespace AMD {
class GPUInfoPMLegacyDataSource
: public IDataSource<std::string, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "power_method";
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
      else {
        LOG(WARNING) << fmt::format(
            "Cannot retrieve device power_method from {}", filePath.c_str());
      }
    }

    return false;
  }
};

class GPUInfoPMDPMDataSource
: public IDataSource<std::string, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "power_dpm_force_performance_level";
  }

  bool read(std::string &, std::filesystem::path const &path) override
  {
    // newer gpus uses 'power_dpm_force_performance_level' file
    auto const filePath = path / source();
    if (Utils::File::isSysFSEntryValid(filePath))
      return true;

    return false;
  }
};
} // namespace AMD

AMD::GPUInfoPM::GPUInfoPM(
    std::vector<std::shared_ptr<IDataSource<std::string, std::filesystem::path const>>>
        &&dataSources) noexcept
: dataSources_(std::move(dataSources))
{
}

std::vector<std::pair<std::string, std::string>>
AMD::GPUInfoPM::provideInfo(Vendor, int, IGPUInfo::Path const &,
                            IHWIDTranslator const &)
{
  return {};
}

std::vector<std::string>
AMD::GPUInfoPM::provideCapabilities(Vendor vendor, int,
                                    IGPUInfo::Path const &path)
{
  std::vector<std::string> caps;

  if (vendor == Vendor::AMD) {
    for (auto &dataSource : dataSources_) {
      std::string data;
      if (dataSource->read(data, path.sys)) {
        // Older gpus have 'power_method' file
        if (dataSource->source() == "power_method") {
          if (data == "dynpm" || data == "profile")
            caps.emplace_back(GPUInfoPM::Legacy);
          else if (data == "dpm")
            caps.emplace_back(GPUInfoPM::Radeon);
        }
        // Newer gpus uses 'power_dpm_force_performance_level' file
        else if (dataSource->source() == "power_dpm_force_performance_level") {
          caps.emplace_back(GPUInfoPM::Amdgpu);
        }
      }
    }
  }

  return caps;
}

bool AMD::GPUInfoPM::registered_ =
    InfoProviderRegistry::add(std::make_unique<AMD::GPUInfoPM>(
        std::vector<
            std::shared_ptr<IDataSource<std::string, std::filesystem::path const>>>(
            {std::make_shared<AMD::GPUInfoPMLegacyDataSource>(),
             std::make_shared<AMD::GPUInfoPMDPMDataSource>()})));
