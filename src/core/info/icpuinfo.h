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

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/// Interface to retrieve info of a physical cpu package.
class ICPUInfo
{
 public:
  struct Keys
  {
    static constexpr std::string_view vendorId{"vendorid"};
    static constexpr std::string_view cpuFamily{"cpufamily"};
    static constexpr std::string_view model{"model"};
    static constexpr std::string_view modelName{"modname"};
    static constexpr std::string_view stepping{"stepping"};
    static constexpr std::string_view ucode{"ucodev"};
    static constexpr std::string_view l3Cache{"l3cache"};
    static constexpr std::string_view executionUnits{"exeunits"};
    static constexpr std::string_view cores{"cores"};
    static constexpr std::string_view flags{"flags"};
    static constexpr std::string_view bugs{"bugs"};
    static constexpr std::string_view bogomips{"bogomips"};
  };

  /// An execution unit is exposed by the system as a single cpu.
  struct ExecutionUnit
  {
    ExecutionUnit(int cpuId, int coreId, std::filesystem::path sysPath)
    : cpuId(cpuId)
    , coreId(coreId)
    , sysPath(std::move(sysPath))
    {
    }

    /// cpu id exposed by the system
    int const cpuId;

    /// core id on the physical cpu package in which the cpu resides
    int const coreId;

    /// sysfs path
    std::filesystem::path const sysPath;
  };

  class IProvider
  {
   public:
    virtual std::vector<std::pair<std::string, std::string>>
    provideInfo(int socketId,
                std::vector<ICPUInfo::ExecutionUnit> const &executionUnits) = 0;

    virtual std::vector<std::string> provideCapabilities(
        int socketId,
        std::vector<ICPUInfo::ExecutionUnit> const &executionUnits) = 0;

    virtual ~IProvider() = default;
  };

  class IProviderRegistry
  {
   public:
    virtual std::vector<std::unique_ptr<ICPUInfo::IProvider>> const &
    cpuInfoProviders() const = 0;

    virtual ~IProviderRegistry() = default;
  };

  /// @return socket id
  virtual int socketId() const = 0;

  /// @return available execution units
  virtual std::vector<ICPUInfo::ExecutionUnit> const &executionUnits() const = 0;

  /// @returns collection of keys for the stored gpu information
  virtual std::vector<std::string> keys() const = 0;

  /// @returns string with gpu info or empty string when no info
  /// is mapped to the key
  virtual std::string info(std::string_view key) const = 0;

  /// @returns true when a capability is present
  virtual bool hasCapability(std::string_view name) const = 0;

  /// Initialize cpu info
  /// @param infoProviders information providers
  virtual void initialize(
      std::vector<std::unique_ptr<ICPUInfo::IProvider>> const &infoProviders) = 0;

  virtual ~ICPUInfo() = default;
};
