// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <vector>

class IControl;
class IGPUInfo;
class ISWInfo;

class IGPUControlProvider
{
 public:
  /// Classes that provides instances of Control specializations must
  /// implement this interface.
  class IProvider
  {
   public:
    /// Returns a list with the GPU controls supported by the provider.
    virtual std::vector<std::unique_ptr<IControl>>
    provideGPUControls(IGPUInfo const &gpuInfo, ISWInfo const &swInfo) const = 0;

    virtual ~IProvider() = default;
  };

  virtual std::vector<std::unique_ptr<IGPUControlProvider::IProvider>> const &
  gpuControlProviders() const = 0;

  virtual ~IGPUControlProvider() = default;
};
