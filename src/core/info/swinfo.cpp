// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "swinfo.h"

#include <algorithm>
#include <iterator>
#include <utility>

std::string SWInfo::info(std::string_view key) const
{
  auto const dataIt = info_.find(std::string(key));
  if (dataIt != info_.cend())
    return dataIt->second;

  return std::string{};
}

std::vector<std::string> SWInfo::keys() const
{
  std::vector<std::string> keys;
  keys.reserve(info_.size());

  auto const keySelector = [](auto &pair) { return pair.first; };
  std::transform(info_.cbegin(), info_.cend(), std::back_inserter(keys),
                 keySelector);

  return keys;
}

void SWInfo::initialize(
    std::vector<std::unique_ptr<ISWInfo::IProvider>> const &infoProviders)
{
  for (auto &provider : infoProviders) {
    auto infos = provider->provideInfo();
    for (auto &info : infos)
      info_.insert(std::move(info));
  }
}
