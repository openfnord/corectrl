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
