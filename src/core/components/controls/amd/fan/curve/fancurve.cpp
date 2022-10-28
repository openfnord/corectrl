// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fancurve.h"

#include "common/mathutils.h"
#include "core/components/commonutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>

AMD::FanCurve::FanCurve(
    std::unique_ptr<IDataSource<unsigned int>> &&pwmEnableDataSource,
    std::unique_ptr<IDataSource<unsigned int>> &&pwmDataSource,
    std::unique_ptr<IDataSource<int>> &&tempInputDataSource,
    units::temperature::celsius_t tempMin,
    units::temperature::celsius_t tempMax) noexcept
: Control(false)
, id_(AMD::FanCurve::ItemID)
, pwmEnableDataSource_(std::move(pwmEnableDataSource))
, pwmDataSource_(std::move(pwmDataSource))
, tempInputDataSource_(std::move(tempInputDataSource))
, tempRange_(std::make_pair(tempMin, tempMax))
, fanStop_(false)
, fanStartValue_(54)
, hysteresis_(5)
, lastPwmValue_(std::numeric_limits<unsigned int>::max())
, lastFanStop_(false)
{
  // default curve
  points_.emplace_back(units::temperature::celsius_t(35),
                       units::make_unit<units::concentration::percent_t>(20));
  points_.emplace_back(units::temperature::celsius_t(52),
                       units::make_unit<units::concentration::percent_t>(22));
  points_.emplace_back(units::temperature::celsius_t(67),
                       units::make_unit<units::concentration::percent_t>(30));
  points_.emplace_back(units::temperature::celsius_t(78),
                       units::make_unit<units::concentration::percent_t>(50));
  points_.emplace_back(units::temperature::celsius_t(85),
                       units::make_unit<units::concentration::percent_t>(82));

  Utils::Common::normalizePoints(points_, tempRange_);

  // compute fan start temperature
  fanStartTemp_ = evaluatePwm(std::round(fanStartValue_ / 2.55));
}

void AMD::FanCurve::preInit(ICommandQueue &)
{
}

void AMD::FanCurve::postInit(ICommandQueue &)
{
}

void AMD::FanCurve::init()
{
}

std::string const &AMD::FanCurve::ID() const
{
  return id_;
}

void AMD::FanCurve::importControl(IControl::Importer &i)
{
  auto &fanCurveImporter = dynamic_cast<AMD::FanCurve::Importer &>(i);
  curve(fanCurveImporter.provideFanCurvePoints());
  fanStop(fanCurveImporter.provideFanCurveFanStop());
  fanStartValue(static_cast<unsigned int>(std::round(
      fanCurveImporter.provideFanCurveFanStartValue().to<double>() * 255)));
}

void AMD::FanCurve::exportControl(IControl::Exporter &e) const
{
  auto &fanCurveExporter = dynamic_cast<AMD::FanCurve::Exporter &>(e);
  fanCurveExporter.takeFanCurveTemperatureRange(tempRange_.first,
                                                tempRange_.second);
  fanCurveExporter.takeFanCurvePoints(curve());
  fanCurveExporter.takeFanCurveFanStop(fanStop());
  fanCurveExporter.takeFanCurveFanStartValue(std::round(fanStartValue() / 2.55));
}

void AMD::FanCurve::cleanControl(ICommandQueue &)
{
  lastPwmValue_ = std::numeric_limits<unsigned int>::max();
  lastFanStop_ = false;
}

void AMD::FanCurve::syncControl(ICommandQueue &ctlCmds)
{
  if (pwmEnableDataSource_->read(pwmEnable_) && pwmDataSource_->read(pwm_) &&
      tempInputDataSource_->read(tempInput_)) {

    unsigned int pwmValue;
    if (fanStop() &&
        ((pwm_ > 0 && tempInput_ < fanStartTemp_ - hysteresis_) || // stop it
         (pwm_ == 0 && tempInput_ < fanStartTemp_))) { // don't start it
      pwmValue = 0;
    }
    else {
      if (fanStop() && pwm_ > 0 && tempInput_ < fanStartTemp_)
        pwmValue = fanStartValue_; // cap it to fan start value
      else
        pwmValue = evaluateTemp(units::temperature::celsius_t(tempInput_));
    }

    if (pwmEnable_ != 1) {
      ctlCmds.add({pwmEnableDataSource_->source(), "1"});
      ctlCmds.add({pwmDataSource_->source(), std::to_string(pwmValue)});
    }
    // NOTE pwm_ must be checked as well. The value that the hardware uses
    // doesn't have to match the assigned value through pwm1.
    // See: https://bugs.freedesktop.org/show_bug.cgi?id=110213
    else if ((pwm_ != pwmValue && lastPwmValue_ != pwmValue) ||
             lastFanStop_ != fanStop()) { // force sync
      ctlCmds.add({pwmDataSource_->source(), std::to_string(pwmValue)});
    }

    lastPwmValue_ = pwmValue;
    lastFanStop_ = fanStop();
  }
}

std::vector<AMD::FanCurve::Point> const &AMD::FanCurve::curve() const
{
  return points_;
}

void AMD::FanCurve::curve(std::vector<AMD::FanCurve::Point> const &points)
{
  points_ = points;
  Utils::Common::normalizePoints(points_, tempRange_);
  fanStartTemp_ = evaluatePwm(std::round(fanStartValue_ / 2.55));
}

bool AMD::FanCurve::fanStop() const
{
  return fanStop_;
}

void AMD::FanCurve::fanStop(bool enable)
{
  fanStop_ = enable;
}

unsigned int AMD::FanCurve::fanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanCurve::fanStartValue(unsigned int value)
{
  fanStartValue_ = static_cast<unsigned int>(std::clamp(value, 0u, 255u));
  fanStartTemp_ = evaluatePwm(std::round(fanStartValue_ / 2.55));
}

int AMD::FanCurve::hysteresis() const
{
  return hysteresis_;
}

int AMD::FanCurve::evaluatePwm(units::concentration::percent_t input) const
{
  if (points_.size() > 1) {
    auto pIt = std::find_if(points_.cbegin(), points_.cend(),
                            [=](auto &p) { return p.second > input; });

    auto &p1 = (pIt == points_.cbegin()
                    ? *pIt
                    : (pIt == points_.cend() ? *std::prev(std::prev(pIt))
                                             : *std::prev(pIt)));
    auto &p2 = (pIt == points_.cend()
                    ? *std::prev(pIt)
                    : (pIt == points_.cbegin() ? *std::next(pIt) : *pIt));

    return lerpFromPwm(input, p1, p2);
  }
  else if (points_.size() == 1)
    return static_cast<int>(std::round(points_.front().first.to<double>()));
  else
    return 0;
}

int AMD::FanCurve::lerpFromPwm(units::concentration::percent_t input,
                               AMD::FanCurve::Point const &p1,
                               AMD::FanCurve::Point const &p2) const
{
  input = std::clamp(input, p1.second, p2.second);
  return static_cast<int>(std::round(Utils::Math::lerpY(
      input.to<double>(),
      std::make_pair(p1.first.to<double>(), p1.second.to<double>()),
      std::make_pair(p2.first.to<double>(), p2.second.to<double>()))));
}

unsigned int AMD::FanCurve::evaluateTemp(units::temperature::celsius_t input) const
{
  if (points_.size() > 1) {
    auto pIt = std::find_if(points_.cbegin(), points_.cend(),
                            [=](auto &p) { return p.first > input; });

    auto &p1 = (pIt == points_.cbegin()
                    ? *pIt
                    : (pIt == points_.cend() ? *std::prev(std::prev(pIt))
                                             : *std::prev(pIt)));
    auto &p2 = (pIt == points_.cend()
                    ? *std::prev(pIt)
                    : (pIt == points_.cbegin() ? *std::next(pIt) : *pIt));

    return lerpFromTemp(input, p1, p2);
  }
  else if (points_.size() == 1)
    return static_cast<unsigned int>(
        std::round(points_.front().second.to<double>() * 255));
  else
    return 0;
}

unsigned int AMD::FanCurve::lerpFromTemp(units::temperature::celsius_t input,
                                         AMD::FanCurve::Point const &p1,
                                         AMD::FanCurve::Point const &p2) const
{
  input = std::clamp(input, p1.first, p2.first);
  return static_cast<unsigned int>(std::round(
      Utils::Math::lerpX(
          input.to<double>(),
          std::make_pair(p1.first.to<double>(), p1.second.to<double>()),
          std::make_pair(p2.first.to<double>(), p2.second.to<double>())) *
      255)); // scale to [0, 255] range of pwm1
}
