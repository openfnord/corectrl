//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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

/// Overdrive voltage curve control.
class PMVoltCurve : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_VOLT_CURVE"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMVoltCurveMode() const = 0;
    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMVoltCurvePoint(unsigned int index) const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMVoltCurveModes(std::vector<std::string> const &modes) = 0;
    virtual void takePMVoltCurveMode(std::string const &mode) = 0;
    virtual void takePMVoltCurvePointsRange(
        std::vector<std::pair<
            std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
            std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
            &pointRanges) = 0;
    virtual void takePMVoltCurvePoints(
        std::vector<std::pair<units::frequency::megahertz_t,
                              units::voltage::millivolt_t>> const &curve) = 0;
  };

  PMVoltCurve(std::string &&controlCmdId,
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

  std::string const &controlCmdId() const;

  std::vector<std::string> const &modes() const;
  void mode(std::string const &mode);
  std::string const &mode() const;

  std::vector<std::pair<
      std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
      std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const &
  pointsRange() const;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>> const &
  points() const;

  void point(unsigned int index, units::frequency::megahertz_t freq,
             units::voltage::millivolt_t volt);

  std::string ppOdClkVoltCmd(unsigned int index,
                             units::frequency::megahertz_t freq,
                             units::voltage::millivolt_t volt) const;

 private:
  std::string const id_;
  std::string const controlCmdId_;

  std::unique_ptr<IDataSource<std::vector<std::string>>> const ppOdClkVoltDataSource_;
  std::vector<std::string> ppOdClkVoltLines_;

  enum class Mode { Automatic, Manual };
  std::vector<std::string> const modes_;
  Mode mode_;

  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      preInitPoints_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      initPoints_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      points_;
  std::vector<std::pair<
      std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
      std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
      pointsRange_;
};

} // namespace AMD
