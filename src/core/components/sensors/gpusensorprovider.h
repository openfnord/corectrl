// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "igpusensorprovider.h"
#include <memory>
#include <vector>

class GPUSensorProvider final : public IGPUSensorProvider
{
 public:
  std::vector<std::unique_ptr<IGPUSensorProvider::IProvider>> const &
  gpuSensorProviders() const override;

  static bool
  registerProvider(std::unique_ptr<IGPUSensorProvider::IProvider> &&provider);

 private:
  static std::vector<std::unique_ptr<IGPUSensorProvider::IProvider>> &
  gpuSensorProviders_();
};
