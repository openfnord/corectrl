// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "vendor.h"
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class IHWIDTranslator;

class IGPUInfo
{
 public:
  struct Keys
  {
    static constexpr std::string_view vendorID{"vendorid"};
    static constexpr std::string_view deviceID{"deviceid"};
    static constexpr std::string_view subvendorID{"svendorid"};
    static constexpr std::string_view subdeviceID{"sdeviceid"};
    static constexpr std::string_view vendorName{"vendor"};
    static constexpr std::string_view deviceName{"device"};
    static constexpr std::string_view subdeviceName{"sdevice"};
    static constexpr std::string_view pciSlot{"pcislot"};
    static constexpr std::string_view driver{"driver"};
    static constexpr std::string_view revision{"revision"};
    static constexpr std::string_view memory{"memory"};
    static constexpr std::string_view uniqueID{"uniqueid"};
  };

  struct Path
  {
    Path(std::filesystem::path sys, std::filesystem::path dev) noexcept
    : sys(std::move(sys))
    , dev(std::move(dev))
    {
    }
    std::filesystem::path const sys;
    std::filesystem::path const dev;
  };

  class IProvider
  {
   public:
    virtual std::vector<std::pair<std::string, std::string>>
    provideInfo(Vendor vendor, int gpuIndex, IGPUInfo::Path const &path,
                IHWIDTranslator const &hwIDTranslator) = 0;

    virtual std::vector<std::string>
    provideCapabilities(Vendor vendor, int gpuIndex,
                        IGPUInfo::Path const &path) = 0;

    virtual ~IProvider() = default;
  };

  class IProviderRegistry
  {
   public:
    virtual std::vector<std::unique_ptr<IGPUInfo::IProvider>> const &
    gpuInfoProviders() const = 0;

    virtual ~IProviderRegistry() = default;
  };

  /// @returns gpu vendor id
  virtual Vendor vendor() const = 0;

  /// @returns gpu index
  virtual int index() const = 0;

  /// @returns gpu Path
  virtual IGPUInfo::Path const &path() const = 0;

  /// @returns collection of keys for the stored gpu information
  virtual std::vector<std::string> keys() const = 0;

  /// @returns string with gpu info or empty string when no info
  /// is mapped to the key
  virtual std::string info(std::string_view key) const = 0;

  /// @returns true when the capability is present
  virtual bool hasCapability(std::string_view name) const = 0;

  /// Initialize gpu info
  /// @param infoProviders information providers
  /// @param hwidTranslator hwid code translator
  virtual void initialize(
      std::vector<std::unique_ptr<IGPUInfo::IProvider>> const &infoProviders,
      IHWIDTranslator const &hwidTranslator) = 0;

  virtual ~IGPUInfo() = default;
};
