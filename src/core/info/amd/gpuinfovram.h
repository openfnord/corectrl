// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

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
