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
#include "gpuinforevision.h"

#include "../infoproviderregistry.h"
#include "common/fileutils.h"
#include "common/stringutils.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <algorithm>
#include <cctype>
#include <utility>

class GPUInfoRevisionDataSource
: public IDataSource<std::string, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "revision";
  }

  bool read(std::string &data, std::filesystem::path const &path) override
  {
    auto const filePath = path / source();
    auto const lines = Utils::File::readFileLines(filePath);
    if (!lines.empty()) {
      data = lines.front();
      return true;
    }

    LOG(WARNING) << fmt::format("Cannot retrieve device revision from {}",
                                filePath.c_str());
    return false;
  }
};

GPUInfoRevision::GPUInfoRevision(
    std::unique_ptr<IDataSource<std::string, std::filesystem::path const>>
        &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
GPUInfoRevision::provideInfo(Vendor, int, IGPUInfo::Path const &path,
                             IHWIDTranslator const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  std::string data;
  if (dataSource_->read(data, path.sys)) {
    std::string rev(Utils::String::cleanPrefix(data, "0x"));
    std::transform(rev.cbegin(), rev.cend(), rev.begin(), ::toupper);
    info.emplace_back(IGPUInfo::Keys::revision, std::move(rev));
  }

  return info;
}

std::vector<std::string>
GPUInfoRevision::provideCapabilities(Vendor, int, IGPUInfo::Path const &)
{
  return {};
}

bool GPUInfoRevision::registered_ =
    InfoProviderRegistry::add(std::make_unique<GPUInfoRevision>(
        std::make_unique<GPUInfoRevisionDataSource>()));
