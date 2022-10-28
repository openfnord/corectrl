// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "../igpuinfo.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

template<typename...>
class IDataSource;

class GPUInfoUevent final : public IGPUInfo::IProvider
{
 public:
  GPUInfoUevent(std::unique_ptr<
                IDataSource<std::vector<std::string>, std::filesystem::path const>>
                    &&dataSource) noexcept;

  std::vector<std::pair<std::string, std::string>>
  provideInfo(Vendor vendor, int gpuIndex, IGPUInfo::Path const &path,
              IHWIDTranslator const &hwIDTranslator) override;

  std::vector<std::string>
  provideCapabilities(Vendor vendor, int, IGPUInfo::Path const &path) override;

 private:
  std::unique_ptr<IDataSource<std::vector<std::string>,
                              std::filesystem::path const>> const dataSource_;
  static bool registered_;
};
