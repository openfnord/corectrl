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
#include "gpuinfopmoverdrive.h"

#include "common/fileutils.h"
#include "core/components/amdutils.h"
#include "core/idatasource.h"
#include "core/info/infoproviderregistry.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <algorithm>
#include <utility>

namespace AMD {

class GPUInfoPMOverdriveDataSource
: public IDataSource<std::vector<std::string>, std::filesystem::path const>
{
 public:
  std::string source() const override
  {
    return "pp_od_clk_voltage";
  }

  bool read(std::vector<std::string> &data,
            std::filesystem::path const &path) override
  {
    auto const filePath = path / source();
    if (Utils::File::isSysFSEntryValid(filePath)) {
      auto lines = Utils::File::readFileLines(filePath);
      if (!lines.empty()) {
        data = lines;
        return true;
      }
    }

    return false;
  }
};
} // namespace AMD

AMD::GPUInfoPMOverdrive::GPUInfoPMOverdrive(
    std::unique_ptr<IDataSource<std::vector<std::string>, std::filesystem::path const>>
        &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
AMD::GPUInfoPMOverdrive::provideInfo(Vendor, int, IGPUInfo::Path const &,
                                     IHWIDTranslator const &)
{
  return {};
}

std::vector<std::string>
AMD::GPUInfoPMOverdrive::provideCapabilities(Vendor vendor, int,
                                             IGPUInfo::Path const &path)
{
  std::vector<std::string> cap;

  if (vendor == Vendor::AMD) {

    std::vector<std::string> data;
    if (dataSource_->read(data, path.sys)) {

      if (Utils::AMD::hasOverdriveClkVoltControl(data))
        cap.emplace_back(GPUInfoPMOverdrive::ClkVolt);
      else if (Utils::AMD::hasOverdriveClkControl(data))
        cap.emplace_back(GPUInfoPMOverdrive::Clk);

      if (Utils::AMD::hasOverdriveVoltCurveControl(data))
        cap.emplace_back(GPUInfoPMOverdrive::VoltCurve);
    }
  }

  return cap;
}

bool AMD::GPUInfoPMOverdrive::registered_ =
    InfoProviderRegistry::add(std::make_unique<AMD::GPUInfoPMOverdrive>(
        std::make_unique<AMD::GPUInfoPMOverdriveDataSource>()));
