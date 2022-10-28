// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class ISWInfo
{
 public:
  struct Keys
  {
    static constexpr std::string_view kernelVersion{"kernelv"};
    static constexpr std::string_view mesaVersion{"mesav"};
  };

  /// ISWInfo provider classes must implement this interface
  class IProvider
  {
   public:
    virtual std::vector<std::pair<std::string, std::string>> provideInfo() = 0;
    virtual ~IProvider() = default;
  };

  /// ISWInfo::IProvider registry classes must implement this interface
  class IProviderRegistry
  {
   public:
    virtual std::vector<std::unique_ptr<ISWInfo::IProvider>> const &
    swInfoProviders() const = 0;

    virtual ~IProviderRegistry() = default;
  };

  /// @returns SWInfo data or empty data if there is no info mapped to key
  virtual std::string info(std::string_view key) const = 0;

  /// @returns collection of keys for the stored software information
  virtual std::vector<std::string> keys() const = 0;

  /// Initialize software info
  /// @param infoProviders information providers
  virtual void initialize(
      std::vector<std::unique_ptr<ISWInfo::IProvider>> const &infoProviders) = 0;

  virtual ~ISWInfo() = default;
};
