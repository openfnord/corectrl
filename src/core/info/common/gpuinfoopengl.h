// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "../igpuinfo.h"
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

class GPUInfoOpenGL final : public IGPUInfo::IProvider
{
 public:
  struct Keys
  {
    static constexpr std::string_view coreVersion{"glcorev"};
    static constexpr std::string_view compatVersion{"glcompv"};
  };

  GPUInfoOpenGL(
      std::unique_ptr<IDataSource<std::string, int const>> &&dataSource) noexcept;

  std::vector<std::pair<std::string, std::string>>
  provideInfo(Vendor vendor, int gpuIndex, IGPUInfo::Path const &path,
              IHWIDTranslator const &hwIDTranslator) override;

  std::vector<std::string>
  provideCapabilities(Vendor vendor, int, IGPUInfo::Path const &path) override;

 private:
  std::string findItem(std::string const &src, std::string_view itemStr,
                       size_t pos) const;

  std::unique_ptr<IDataSource<std::string, int const>> const dataSource_;
  static bool registered_;
};
