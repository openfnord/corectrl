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

#include "core/components/controls/control.h"
#include "units/units.h"
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class IPpDpmHandler;

/// A frequency & voltage control for state based ASICs, using
/// fixed frequency and voltage on each state (pre-vega20).
class PMFVState : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FV_STATE"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMFVStateGPUVoltMode() const = 0;
    virtual std::string const &providePMFVStateMemVoltMode() const = 0;

    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFVStateGPUState(unsigned int index) const = 0;
    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFVStateMemState(unsigned int index) const = 0;

    virtual std::vector<unsigned int> providePMFVStateGPUActiveStates() const = 0;
    virtual std::vector<unsigned int> providePMFVStateMemActiveStates() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFVStateVoltModes(std::vector<std::string> const &modes) = 0;
    virtual void takePMFVStateGPUVoltMode(std::string const &mode) = 0;
    virtual void takePMFVStateMemVoltMode(std::string const &mode) = 0;

    virtual void takePMFVStateVoltRange(units::voltage::millivolt_t min,
                                        units::voltage::millivolt_t max) = 0;
    virtual void takePMFVStateGPURange(units::frequency::megahertz_t min,
                                       units::frequency::megahertz_t max) = 0;
    virtual void takePMFVStateMemRange(units::frequency::megahertz_t min,
                                       units::frequency::megahertz_t max) = 0;

    virtual void takePMFVStateGPUStates(
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> const &states) = 0;
    virtual void takePMFVStateMemStates(
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> const &states) = 0;

    virtual void
    takePMFVStateGPUActiveStates(std::vector<unsigned int> const &indices) = 0;
    virtual void
    takePMFVStateMemActiveStates(std::vector<unsigned int> const &indices) = 0;
  };

  PMFVState(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
            std::unique_ptr<IDataSource<std::vector<std::string>>>
                &&ppOdClkVoltDataSource,
            std::unique_ptr<IPpDpmHandler> &&ppDpmSclkHandler,
            std::unique_ptr<IPpDpmHandler> &&ppDpmMclkHandler) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) override;
  void syncControl(ICommandQueue &ctlCmds) override;

  std::vector<std::string> const &voltModes() const;

  void gpuVoltMode(std::string const &mode);
  std::string const &gpuVoltMode() const;

  void memVoltMode(std::string const &mode);
  std::string const &memVoltMode() const;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
  gpuRange() const;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
  memRange() const;
  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> const &
  voltRange() const;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
  gpuStates() const;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
  memStates() const;

  void gpuState(unsigned int index, units::frequency::megahertz_t freq,
                units::voltage::millivolt_t volt);
  void memState(unsigned int index, units::frequency::megahertz_t freq,
                units::voltage::millivolt_t volt);

  std::string ppOdClkVoltStateCmd(std::string &&type, unsigned int index,
                                  units::frequency::megahertz_t freq,
                                  units::voltage::millivolt_t volt) const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;
  std::unique_ptr<IPpDpmHandler> const ppDpmSclkHandler_;
  std::unique_ptr<IPpDpmHandler> const ppDpmMclkHandler_;

  std::string perfLevelPreInitValue_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      gpuPreInitStates_;
  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      memPreInitStates_;

  std::vector<std::string> const voltModes_;

  enum class VoltMode { Automatic, Manual };
  VoltMode gpuVoltMode_;
  VoltMode memVoltMode_;

  std::string perfLevelValue_;
  std::vector<std::string> ppOdClkVoltLines_;

  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      gpuStates_;
  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      memStates_;

  std::unordered_map<unsigned int, units::voltage::millivolt_t> gpuInitVoltages_;
  std::unordered_map<unsigned int, units::voltage::millivolt_t> memInitVoltages_;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> gpuRange_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> memRange_;
  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> voltRange_;
};

} // namespace AMD
