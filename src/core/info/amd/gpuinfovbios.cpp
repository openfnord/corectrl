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
#include "gpuinfovbios.h"

#include "common/fileutils.h"
#include "core/idatasource.h"
#include "core/info/infoproviderregistry.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <algorithm>
#include <cctype>
#include <utility>

namespace AMD {
class GPUInfoVbiosDataSource
: public IDataSource<std::string, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "vbios_version";
  }

  bool read(std::string &data, std::filesystem::path const &path) override
  {
    auto const filePath = path / source();
    auto const lines = Utils::File::readFileLines(filePath);
    if (!lines.empty()) {
      data = lines.front();
      return true;
    }

    LOG(WARNING) << fmt::format("Cannot retrieve bios version from {}",
                                filePath.c_str());
    return false;
  }
};
} // namespace AMD

AMD::GPUInfoVbios::GPUInfoVbios(
    std::unique_ptr<IDataSource<std::string, std::filesystem::path const>>
        &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
AMD::GPUInfoVbios::provideInfo(Vendor vendor, int, IGPUInfo::Path const &path,
                               IHWIDTranslator const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  if (vendor == Vendor::AMD) {
    std::string data;
    if (dataSource_->read(data, path.sys)) {
      std::transform(data.cbegin(), data.cend(), data.begin(), ::toupper);
      info.emplace_back(GPUInfoVbios::version, std::move(data));
    }
  }

  return info;
}

std::vector<std::string>
AMD::GPUInfoVbios::provideCapabilities(Vendor, int, IGPUInfo::Path const &)
{
  return {};
}

bool AMD::GPUInfoVbios::registered_ =
    InfoProviderRegistry::add(std::make_unique<AMD::GPUInfoVbios>(
        std::make_unique<GPUInfoVbiosDataSource>()));
