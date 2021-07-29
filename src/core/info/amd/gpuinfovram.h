//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
#pragma once

#include "../igpuinfo.h"
#include "units/units.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

template<typename...>
class IDataSource;

class GPUInfoVRam final : public IGPUInfo::IProvider
{
 public:
  GPUInfoVRam(
      std::unique_ptr<IDataSource<std::string>> &&kernelVersionDataSource,
      std::unique_ptr<IDataSource<std::vector<std::string>, std::filesystem::path const>>
          &&driverDataSource,
      std::unique_ptr<IDataSource<units::data::megabyte_t, std::filesystem::path const>>
          &&radeonDataSource,
      std::unique_ptr<IDataSource<units::data::megabyte_t, std::filesystem::path const>>
          &&amdgpuDataSource) noexcept;

  std::vector<std::pair<std::string, std::string>>
  provideInfo(Vendor vendor, int gpuIndex, IGPUInfo::Path const &path,
              IHWIDTranslator const &hwIDTranslator) override;

  std::vector<std::string>
  provideCapabilities(Vendor vendor, int, IGPUInfo::Path const &path) override;

 private:
  std::tuple<int, int, int> readKernelVersion() const;
  std::string readDriver(std::filesystem::path const &path) const;

  std::unique_ptr<IDataSource<std::string>> const kernelVersionDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>, std::filesystem::path const>> const
      driverDataSource_;
  std::unique_ptr<IDataSource<units::data::megabyte_t, std::filesystem::path const>> const
      radeonDataSource_;
  std::unique_ptr<IDataSource<units::data::megabyte_t, std::filesystem::path const>> const
      amdgpuDataSource_;

  static bool registered_;
};
