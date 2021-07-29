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
