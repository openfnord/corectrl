// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/info/iswinfo.h"
#include <string>
#include <unordered_map>

class SWInfo final : public ISWInfo
{
 public:
  std::string info(std::string_view key) const override;
  std::vector<std::string> keys() const override;
  void initialize(std::vector<std::unique_ptr<ISWInfo::IProvider>> const
                      &infoProviders) override;

 private:
  std::unordered_map<std::string, std::string> info_;
};
