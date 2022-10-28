// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <vector>

class IControl;
class ICPUInfo;
class ISWInfo;

class ICPUControlProvider
{
 public:
  /// Classes that provides instances of Control specializations must implement
  /// this interface.
  class IProvider
  {
   public:
    /// Returns a list with the CPU controls supported by the provider.
    virtual std::vector<std::unique_ptr<IControl>>
    provideCPUControls(ICPUInfo const &cpuInfo, ISWInfo const &swInfo) const = 0;

    virtual ~IProvider() = default;
  };

  virtual std::vector<std::unique_ptr<ICPUControlProvider::IProvider>> const &
  cpuControlProviders() const = 0;

  virtual ~ICPUControlProvider() = default;
};
