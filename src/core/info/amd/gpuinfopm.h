// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "../igpuinfo.h"
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

/// AMD Power management GPU info
class GPUInfoPM final : public IGPUInfo::IProvider
{
 public:
  static constexpr std::string_view Legacy{"pmlegacy"};
  static constexpr std::string_view Radeon{"pmradeon"};
  static constexpr std::string_view Amdgpu{"pmamdgpu"};

  GPUInfoPM(std::vector<
            std::shared_ptr<IDataSource<std::string, std::filesystem::path const>>>
                &&dataSources) noexcept;

  std::vector<std::pair<std::string, std::string>>
  provideInfo(Vendor vendor, int gpuIndex, IGPUInfo::Path const &path,
              IHWIDTranslator const &hwIDTranslator) override;

  std::vector<std::string>
  provideCapabilities(Vendor vendor, int gpuIndex,
                      IGPUInfo::Path const &path) override;

 private:
  std::vector<std::shared_ptr<IDataSource<std::string, std::filesystem::path const>>> const
      dataSources_;
  static bool registered_;
};

} // namespace AMD
