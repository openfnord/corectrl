// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

      if (Utils::AMD::hasOverdriveVoltOffsetControl(data))
        cap.emplace_back(GPUInfoPMOverdrive::VoltOffset);
    }
  }

  return cap;
}

bool AMD::GPUInfoPMOverdrive::registered_ =
    InfoProviderRegistry::add(std::make_unique<AMD::GPUInfoPMOverdrive>(
        std::make_unique<AMD::GPUInfoPMOverdriveDataSource>()));
