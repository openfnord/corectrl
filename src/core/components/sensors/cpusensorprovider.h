// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icpusensorprovider.h"
#include <memory>
#include <vector>

class CPUSensorProvider final : public ICPUSensorProvider
{
 public:
  std::vector<std::unique_ptr<ICPUSensorProvider::IProvider>> const &
  cpuSensorProviders() const override;

  static bool
  registerProvider(std::unique_ptr<ICPUSensorProvider::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<ICPUSensorProvider::IProvider>> &
  cpuSensorProviders_();
};
