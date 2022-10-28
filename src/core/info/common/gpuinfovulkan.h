// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "../igpuinfo.h"
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

template<typename...>
class IDataSource;

class GPUInfoVulkan final : public IGPUInfo::IProvider
{
 public:
  struct Keys
  {
    static constexpr std::string_view apiVersion{"vkapiv"};
  };

  GPUInfoVulkan(std::unique_ptr<IDataSource<std::string>> &&dataSource) noexcept;

  std::vector<std::pair<std::string, std::string>>
  provideInfo(Vendor vendor, int gpuIndex, IGPUInfo::Path const &path,
              IHWIDTranslator const &hwIDTranslator) override;

  std::vector<std::string>
  provideCapabilities(Vendor vendor, int, IGPUInfo::Path const &path) override;

 private:
  std::string parseApiVersion(std::string const &src, size_t pos) const;

  std::unique_ptr<IDataSource<std::string>> const dataSource_;
  static bool registered_;
};
