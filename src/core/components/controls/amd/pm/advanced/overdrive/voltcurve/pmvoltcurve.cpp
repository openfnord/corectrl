// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmvoltcurve.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <cstddef>
#include <iterator>

AMD::PMVoltCurve::PMVoltCurve(std::string &&controlCmdId,
                              std::unique_ptr<IDataSource<std::vector<std::string>>>
                                  &&ppOdClkVoltDataSource) noexcept
: Control(true)
, id_(AMD::PMVoltCurve::ItemID)
, controlCmdId_(std::move(controlCmdId))
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
, modes_({"auto", "manual"})
, mode_(AMD::PMVoltCurve::Mode::Automatic)
{
}

void AMD::PMVoltCurve::preInit(ICommandQueue &)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    preInitPoints_ =
        Utils::AMD::parseOverdriveVoltCurve(ppOdClkVoltLines_).value();
  }
}

void AMD::PMVoltCurve::postInit(ICommandQueue &ctlCmds)
{
  for (size_t i = 0; i < preInitPoints_.size(); ++i) {
    auto [freq, volt] = preInitPoints_.at(i);
    ctlCmds.add({ppOdClkVoltDataSource_->source(),
                 ppOdClkVoltCmd(static_cast<unsigned int>(i), freq, volt)});
  }
}

void AMD::PMVoltCurve::init()
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {
    pointsRange_ =
        Utils::AMD::parseOverdriveVoltCurveRange(ppOdClkVoltLines_).value();
    points_ = initPoints_ =
        Utils::AMD::parseOverdriveVoltCurve(ppOdClkVoltLines_).value();
  }
}

std::string const &AMD::PMVoltCurve::ID() const
{
  return id_;
}

void AMD::PMVoltCurve::importControl(IControl::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMVoltCurve::Importer &>(i);

  mode(importer.providePMVoltCurveMode());
  for (size_t i = 0; i < points().size(); ++i) {
    auto [freq, volt] =
        importer.providePMVoltCurvePoint(static_cast<unsigned int>(i));
    point(static_cast<unsigned int>(i), freq, volt);
  }
}

void AMD::PMVoltCurve::exportControl(IControl::Exporter &e) const
{
  auto &pmFVVoltCurveExporter = dynamic_cast<AMD::PMVoltCurve::Exporter &>(e);

  pmFVVoltCurveExporter.takePMVoltCurveModes(modes());
  pmFVVoltCurveExporter.takePMVoltCurveMode(mode());
  pmFVVoltCurveExporter.takePMVoltCurvePointsRange(pointsRange());
  pmFVVoltCurveExporter.takePMVoltCurvePoints(points());
}

void AMD::PMVoltCurve::cleanControl(ICommandQueue &)
{
}

void AMD::PMVoltCurve::syncControl(ICommandQueue &ctlCmds)
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    auto points = Utils::AMD::parseOverdriveVoltCurve(ppOdClkVoltLines_);
    for (size_t i = 0; i < points->size(); ++i) {

      auto [freq, volt] = points->at(i);
      auto [targetFreq, targetVolt] = mode_ == AMD::PMVoltCurve::Mode::Automatic
                                          ? initPoints_.at(i)
                                          : points_.at(i);
      if (freq != targetFreq || volt != targetVolt) {
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltCmd(static_cast<unsigned int>(i), targetFreq,
                                    targetVolt)});
      }
    }
  }
}

std::string const &AMD::PMVoltCurve::controlCmdId() const
{
  return controlCmdId_;
}

std::vector<std::string> const &AMD::PMVoltCurve::modes() const
{
  return modes_;
}

void AMD::PMVoltCurve::mode(std::string const &mode)
{
  mode_ = mode == modes_[0] ? AMD::PMVoltCurve::Mode::Automatic
                            : AMD::PMVoltCurve::Mode::Manual;
}

std::string const &AMD::PMVoltCurve::mode() const
{
  return modes_[static_cast<unsigned int>(mode_)];
}

std::vector<std::pair<
    std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
    std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const &
AMD::PMVoltCurve::pointsRange() const
{
  return pointsRange_;
}

std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>> const &
AMD::PMVoltCurve::points() const
{
  return points_;
}

void AMD::PMVoltCurve::point(unsigned int index,
                             units::frequency::megahertz_t freq,
                             units::voltage::millivolt_t volt)
{
  if (index < points_.size()) {
    auto [freqRange, voltRange] = pointsRange_.at(index);
    auto &[pointFreq, pointVolt] = points_.at(index);
    pointFreq = std::clamp(freq, freqRange.first, freqRange.second);
    pointVolt = std::clamp(volt, voltRange.first, voltRange.second);
  }
}

std::string AMD::PMVoltCurve::ppOdClkVoltCmd(unsigned int index,
                                             units::frequency::megahertz_t freq,
                                             units::voltage::millivolt_t volt) const
{
  std::string cmd;
  cmd.reserve(16);
  cmd.append(controlCmdId())
      .append(" ")
      .append(std::to_string(index))
      .append(" ")
      .append(std::to_string(freq.to<unsigned int>()))
      .append(" ")
      .append(std::to_string(volt.to<unsigned int>()));

  return cmd;
}
