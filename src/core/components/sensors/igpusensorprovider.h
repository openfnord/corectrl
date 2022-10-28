// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <vector>

class ISensor;
class IGPUInfo;
class ISWInfo;

class IGPUSensorProvider
{
 public:
  /// Classes that provides instances of Sensor specializations must
  /// implement this interface.
  class IProvider
  {
   public:
    /// Returns a list with the GPU sensors supported by the provider.
    virtual std::vector<std::unique_ptr<ISensor>>
    provideGPUSensors(IGPUInfo const &gpuInfo, ISWInfo const &swInfo) const = 0;

    virtual ~IProvider() = default;
  };

  virtual std::vector<std::unique_ptr<IGPUSensorProvider::IProvider>> const &
  gpuSensorProviders() const = 0;

  virtual ~IGPUSensorProvider() = default;
};
