//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
