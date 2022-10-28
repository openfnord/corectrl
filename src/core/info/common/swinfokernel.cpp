// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "swinfokernel.h"

#include "../infoproviderregistry.h"
#include "common/stringutils.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "swinfokerneldatasource.h"
#include <string_view>
#include <utility>

SWInfoKernel::SWInfoKernel(
    std::unique_ptr<IDataSource<std::string>> &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>> SWInfoKernel::provideInfo()
{
  std::vector<std::pair<std::string, std::string>> info;

  std::string data;
  dataSource_->read(data);
  data = Utils::String::parseKernelProcVersion(data).value_or("0.0.0");

  info.emplace_back(ISWInfo::Keys::kernelVersion, data);

  return info;
}

bool const SWInfoKernel::registered_ = InfoProviderRegistry::add(
    std::make_unique<SWInfoKernel>(std::make_unique<SWInfoKernelDataSource>()));
