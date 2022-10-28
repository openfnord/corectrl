// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <vector>

class ISensor;
class ICPUInfo;
class ISWInfo;

class ICPUSensorProvider
{
 public:
  /// Classes that provides instances of Sensor specializations must
  /// implement this interface.
  class IProvider
  {
   public:
    /// Returns a list with the CPU sensors supported by the provider.
    virtual std::vector<std::unique_ptr<ISensor>>
    provideCPUSensors(ICPUInfo const &cpuInfo, ISWInfo const &swInfo) const = 0;

    virtual ~IProvider() = default;
  };

  virtual std::vector<std::unique_ptr<ICPUSensorProvider::IProvider>> const &
  cpuSensorProviders() const = 0;

  virtual ~ICPUSensorProvider() = default;
};
