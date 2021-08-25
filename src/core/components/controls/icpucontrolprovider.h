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
