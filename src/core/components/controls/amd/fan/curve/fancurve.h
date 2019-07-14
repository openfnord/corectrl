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
#include <string_view>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class FanCurve : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_FAN_CURVE"};

  using Point =
      std::pair<units::temperature::celsius_t, units::concentration::percent_t>;

  class Importer : public IControl::Importer
  {
   public:
    virtual std::vector<FanCurve::Point> const &provideFanCurvePoints() const = 0;
    virtual bool provideFanCurveFanStop() const = 0;
    virtual units::concentration::percent_t
    provideFanCurveFanStartValue() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void
    takeFanCurvePoints(std::vector<FanCurve::Point> const &points) = 0;
    virtual void takeFanCurveFanStop(bool enabled) = 0;
    virtual void
    takeFanCurveFanStartValue(units::concentration::percent_t value) = 0;
    virtual void
    takeFanCurveTemperatureRange(units::temperature::celsius_t min,
                                 units::temperature::celsius_t max) = 0;
  };

  FanCurve(std::unique_ptr<IDataSource<unsigned int>> &&pwmEnableDataSource,
           std::unique_ptr<IDataSource<unsigned int>> &&pwmDataSource,
           std::unique_ptr<IDataSource<int>> &&tempInputDataSource,
           units::temperature::celsius_t tempMin,
           units::temperature::celsius_t tempMax) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  std::vector<FanCurve::Point> const &curve() const;
  void curve(std::vector<FanCurve::Point> const &points);

  bool fanStop() const;
  void fanStop(bool active);

  unsigned int fanStartValue() const;
  void fanStartValue(unsigned int value);

  int hysteresis() const;

  int evaluatePwm(units::concentration::percent_t input) const;
  int lerpFromPwm(units::concentration::percent_t input,
                  FanCurve::Point const &p1, FanCurve::Point const &p2) const;

  unsigned int evaluateTemp(units::temperature::celsius_t input) const;
  unsigned int lerpFromTemp(units::temperature::celsius_t input,
                            FanCurve::Point const &p1,
                            FanCurve::Point const &p2) const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<unsigned int>> const pwmEnableDataSource_;
  std::unique_ptr<IDataSource<unsigned int>> const pwmDataSource_;
  std::unique_ptr<IDataSource<int>> const tempInputDataSource_;
  units::temperature::celsius_t const tempMin_;
  units::temperature::celsius_t const tempMax_;

  unsigned int pwmEnable_;
  unsigned int pwm_;
  int tempInput_;

  bool fanStop_;
  unsigned int fanStartValue_;
  int fanStartTemp_;
  int const hysteresis_;

  unsigned int lastPwmValue_;
  bool lastFanStop_;

  std::vector<FanCurve::Point> points_;
};

} // namespace AMD
