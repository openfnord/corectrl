// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "../iswinfo.h"
#include <memory>
#include <string>

template<typename...>
class IDataSource;

class SWInfoMesa final : public ISWInfo::IProvider
{
 public:
  SWInfoMesa(std::unique_ptr<IDataSource<std::string>> &&dataSource) noexcept;

  std::vector<std::pair<std::string, std::string>> provideInfo() override;

 private:
  std::unique_ptr<IDataSource<std::string>> const dataSource_;
  static bool const registered_;
};
