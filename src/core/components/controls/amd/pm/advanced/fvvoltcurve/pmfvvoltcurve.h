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
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

/// A frequency & voltage control for state based ASICs, using
/// fixed frequency for each state and a single voltage curve (vega20+).
class PMFVVoltCurve : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FV_VOLTCURVE"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMFVVoltCurveVoltMode() const = 0;

    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFVVoltCurveVoltCurvePoint(unsigned int index) const = 0;

    virtual units::frequency::megahertz_t
    providePMFVVoltCurveGPUState(unsigned int index) const = 0;
    virtual units::frequency::megahertz_t
    providePMFVVoltCurveMemState(unsigned int index) const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void
    takePMFVVoltCurveVoltModes(std::vector<std::string> const &modes) = 0;
    virtual void takePMFVVoltCurveVoltMode(std::string const &mode) = 0;

    virtual void takePMFVVoltCurveVoltRange(
        std::vector<std::pair<
            std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
            std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
            &pointRanges) = 0;
    virtual void takePMFVVoltCurveGPURange(units::frequency::megahertz_t min,
                                           units::frequency::megahertz_t max) = 0;
    virtual void takePMFVVoltCurveMemRange(units::frequency::megahertz_t min,
                                           units::frequency::megahertz_t max) = 0;

    virtual void takePMFVVoltCurveVoltCurve(
        std::vector<std::pair<units::frequency::megahertz_t,
                              units::voltage::millivolt_t>> const &curve) = 0;

    virtual void takePMFVVoltCurveGPUStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
    virtual void takePMFVVoltCurveMemStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
  };

  PMFVVoltCurve(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
                std::unique_ptr<IDataSource<std::vector<std::string>>>
                    &&ppOdClkVoltDataSource) noexcept;

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

  void voltMode(std::string const &mode);
  std::string const &voltMode() const;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
  gpuRange() const;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
  memRange() const;
  std::vector<std::pair<
      std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
      std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const &
  voltRange() const;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>> const &
  voltCurve() const;

  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
  gpuStates() const;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
  memStates() const;

  void voltCurvePoint(unsigned int pointIndex,
                      units::frequency::megahertz_t freq,
                      units::voltage::millivolt_t volt);

  void gpuState(unsigned int index, units::frequency::megahertz_t freq);
  void memState(unsigned int index, units::frequency::megahertz_t freq);

  std::string ppOdClkVoltStateCmd(std::string &&type, unsigned int index,
                                  units::frequency::megahertz_t freq) const;
  std::string ppOdClkVoltCurveCmd(unsigned int pointIndex,
                                  units::frequency::megahertz_t freq,
                                  units::voltage::millivolt_t volt) const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;

  std::string perfLevelPreInitValue_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> gpuPreInitStates_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> memPreInitStates_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurvePreInitPoints_;

  std::vector<std::string> const voltModes_;

  enum class VoltMode { Automatic, Manual };
  VoltMode voltMode_;

  std::string perfLevelEntry_;
  std::vector<std::string> ppOdClkVoltLines_;

  std::map<unsigned int, units::frequency::megahertz_t> gpuStates_;
  std::map<unsigned int, units::frequency::megahertz_t> memStates_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurve_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      initVoltCurve_;

  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> gpuRange_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> memRange_;
  std::vector<std::pair<
      std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
      std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
      voltRange_;
};

} // namespace AMD
