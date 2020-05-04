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
#include "pmfvvoltcurve.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <cstddef>
#include <iterator>

AMD::PMFVVoltCurve::PMFVVoltCurve(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>>>
        &&ppOdClkVoltDataSource) noexcept
: Control(true)
, id_(AMD::PMFVVoltCurve::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
, ppOdClkVoltDataSource_(std::move(ppOdClkVoltDataSource))
, voltModes_({"auto", "manual"})
, voltMode_(AMD::PMFVVoltCurve::VoltMode::Automatic)
{
}

void AMD::PMFVVoltCurve::preInit(ICommandQueue &ctlCmds)
{
  perfLevelDataSource_->read(perfLevelPreInitValue_);

  ppOdClkVoltDataSource_->read(ppOdClkVoltLines_);
  gpuPreInitStates_ =
      Utils::AMD::parseOdClkVoltCurveStates("SCLK", ppOdClkVoltLines_).value();
  memPreInitStates_ =
      Utils::AMD::parseOdClkVoltCurveStates("MCLK", ppOdClkVoltLines_).value();
  voltCurvePreInitPoints_ =
      Utils::AMD::parseOdClkVoltCurvePoints(ppOdClkVoltLines_).value();

  cleanControl(ctlCmds);
}

void AMD::PMFVVoltCurve::postInit(ICommandQueue &ctlCmds)
{
  ctlCmds.add({perfLevelDataSource_->source(), perfLevelPreInitValue_});

  if (perfLevelPreInitValue_ == "manual") {

    for (auto [index, freq] : gpuPreInitStates_)
      ctlCmds.add({ppOdClkVoltDataSource_->source(),
                   ppOdClkVoltStateCmd("s", index, freq)});

    for (auto [index, freq] : memPreInitStates_)
      ctlCmds.add({ppOdClkVoltDataSource_->source(),
                   ppOdClkVoltStateCmd("m", index, freq)});

    for (size_t i = 0; i < voltCurvePreInitPoints_.size(); ++i) {
      auto [freq, volt] = voltCurvePreInitPoints_.at(i);
      ctlCmds.add(
          {ppOdClkVoltDataSource_->source(),
           ppOdClkVoltCurveCmd(static_cast<unsigned int>(i), freq, volt)});
    }

    ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});
  }
}

void AMD::PMFVVoltCurve::init()
{
  if (ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    gpuRange_ =
        Utils::AMD::parseOdClkVoltStateClkRange("SCLK", ppOdClkVoltLines_).value();
    memRange_ =
        Utils::AMD::parseOdClkVoltStateClkRange("MCLK", ppOdClkVoltLines_).value();

    auto voltageRange =
        Utils::AMD::parseOdClkVoltCurveVoltRange(ppOdClkVoltLines_);
    voltRange_.reserve(voltageRange->size());
    for (auto &vRange : voltageRange.value())
      // NOTE vega20+ uses gpu clock range as clock range for every point
      voltRange_.emplace_back(std::make_pair(gpuRange_, vRange));

    auto gpuStates = Utils::AMD::parseOdClkVoltCurveStates("SCLK",
                                                           ppOdClkVoltLines_);
    auto [gpuMin, gpuMax] = gpuRange_;
    for (auto [index, freq] : gpuStates.value())
      gpuStates_.emplace(index, (std::clamp(freq, gpuMin, gpuMax)));

    auto memStates = Utils::AMD::parseOdClkVoltCurveStates("MCLK",
                                                           ppOdClkVoltLines_);
    auto [memMin, memMax] = memRange_;
    for (auto [index, freq] : memStates.value())
      memStates_.emplace(index, std::clamp(freq, memMin, memMax));

    auto voltCurve = Utils::AMD::parseOdClkVoltCurvePoints(ppOdClkVoltLines_);
    voltCurve_ = initVoltCurve_ = voltCurve.value();
  }
}

std::string const &AMD::PMFVVoltCurve::ID() const
{
  return id_;
}

void AMD::PMFVVoltCurve::importControl(IControl::Importer &i)
{
  auto &pmFVVoltCurveImporter = dynamic_cast<AMD::PMFVVoltCurve::Importer &>(i);

  voltMode(pmFVVoltCurveImporter.providePMFVVoltCurveVoltMode());

  for (size_t i = 0; i < voltCurve().size(); ++i) {
    auto [freq, volt] = pmFVVoltCurveImporter.providePMFVVoltCurveVoltCurvePoint(
        static_cast<unsigned int>(i));
    voltCurvePoint(static_cast<unsigned int>(i), freq, volt);
  }

  for (auto [index, _] : gpuStates_)
    gpuState(index, pmFVVoltCurveImporter.providePMFVVoltCurveGPUState(index));

  for (auto [index, _] : memStates_)
    memState(index, pmFVVoltCurveImporter.providePMFVVoltCurveMemState(index));
}

void AMD::PMFVVoltCurve::exportControl(IControl::Exporter &e) const
{
  auto &pmFVVoltCurveExporter = dynamic_cast<AMD::PMFVVoltCurve::Exporter &>(e);

  pmFVVoltCurveExporter.takePMFVVoltCurveVoltModes(voltModes());
  pmFVVoltCurveExporter.takePMFVVoltCurveVoltMode(voltMode());

  auto [gpuMim, gpuMax] = gpuRange();
  pmFVVoltCurveExporter.takePMFVVoltCurveGPURange(gpuMim, gpuMax);
  auto [memMim, memMax] = memRange();
  pmFVVoltCurveExporter.takePMFVVoltCurveMemRange(memMim, memMax);
  pmFVVoltCurveExporter.takePMFVVoltCurveVoltRange(voltRange());

  pmFVVoltCurveExporter.takePMFVVoltCurveVoltCurve(voltCurve());

  pmFVVoltCurveExporter.takePMFVVoltCurveGPUStates(gpuStates());
  pmFVVoltCurveExporter.takePMFVVoltCurveMemStates(memStates());
}

void AMD::PMFVVoltCurve::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({perfLevelDataSource_->source(), "manual"});

  ctlCmds.add({ppOdClkVoltDataSource_->source(), "r"});
  ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});
}

void AMD::PMFVVoltCurve::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(perfLevelEntry_) &&
      ppOdClkVoltDataSource_->read(ppOdClkVoltLines_)) {

    if (perfLevelEntry_ != "manual") {
      ctlCmds.add({perfLevelDataSource_->source(), "manual"});

      // sclk states
      for (auto [index, freq] : gpuStates_)
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltStateCmd("s", index, freq)});

      // mclk states
      for (auto [index, freq] : memStates_)
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltStateCmd("m", index, freq)});

      // voltage curve
      for (size_t i = 0; i < voltCurve_.size(); ++i) {
        auto [targetFreq,
              targetVolt] = voltMode_ == AMD::PMFVVoltCurve::VoltMode::Automatic
                                ? initVoltCurve_.at(i)
                                : voltCurve_.at(i);
        ctlCmds.add({ppOdClkVoltDataSource_->source(),
                     ppOdClkVoltCurveCmd(static_cast<unsigned int>(i),
                                         targetFreq, targetVolt)});
      }

      ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});
    }
    else {
      bool commit{false};

      auto gpuStates = Utils::AMD::parseOdClkVoltCurveStates("SCLK",
                                                             ppOdClkVoltLines_);
      for (auto [index, freq] : gpuStates.value()) {
        auto targetFreq = gpuStates_.at(index);
        if (freq != targetFreq) {
          ctlCmds.add({ppOdClkVoltDataSource_->source(),
                       ppOdClkVoltStateCmd("s", index, targetFreq)});
          commit = true;
        }
      }

      auto memStates = Utils::AMD::parseOdClkVoltCurveStates("MCLK",
                                                             ppOdClkVoltLines_);
      for (auto [index, freq] : memStates.value()) {
        auto targetFreq = memStates_.at(index);
        if (freq != targetFreq) {
          ctlCmds.add({ppOdClkVoltDataSource_->source(),
                       ppOdClkVoltStateCmd("m", index, targetFreq)});
          commit = true;
        }
      }

      auto voltCurve = Utils::AMD::parseOdClkVoltCurvePoints(ppOdClkVoltLines_);
      for (size_t i = 0; i < voltCurve->size(); ++i) {

        auto [freq, volt] = voltCurve->at(i);
        auto [targetFreq,
              targetVolt] = voltMode_ == AMD::PMFVVoltCurve::VoltMode::Automatic
                                ? initVoltCurve_.at(i)
                                : voltCurve_.at(i);

        if (freq != targetFreq || volt != targetVolt) {
          ctlCmds.add({ppOdClkVoltDataSource_->source(),
                       ppOdClkVoltCurveCmd(static_cast<unsigned int>(i),
                                           targetFreq, targetVolt)});
          commit = true;
        }
      }

      if (commit)
        ctlCmds.add({ppOdClkVoltDataSource_->source(), "c"});
    }
  }
}

std::vector<std::string> const &AMD::PMFVVoltCurve::voltModes() const
{
  return voltModes_;
}

void AMD::PMFVVoltCurve::voltMode(std::string const &mode)
{
  voltMode_ = mode == voltModes_[0] ? AMD::PMFVVoltCurve::VoltMode::Automatic
                                    : AMD::PMFVVoltCurve::VoltMode::Manual;
}

std::string const &AMD::PMFVVoltCurve::voltMode() const
{
  return voltModes_[static_cast<unsigned int>(voltMode_)];
}

std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
AMD::PMFVVoltCurve::gpuRange() const
{
  return gpuRange_;
}

std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const &
AMD::PMFVVoltCurve::memRange() const
{
  return memRange_;
}

std::vector<std::pair<
    std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
    std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const &
AMD::PMFVVoltCurve::voltRange() const
{
  return voltRange_;
}

std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>> const &
AMD::PMFVVoltCurve::voltCurve() const
{
  return voltCurve_;
}

std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
AMD::PMFVVoltCurve::gpuStates() const
{
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;
  states.reserve(gpuStates_.size());
  std::transform(gpuStates_.cbegin(), gpuStates_.cend(),
                 std::back_inserter(states),
                 [](auto &kv) { return std::make_pair(kv.first, kv.second); });
  return states;
}

std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>
AMD::PMFVVoltCurve::memStates() const
{
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states;
  states.reserve(memStates_.size());
  std::transform(memStates_.cbegin(), memStates_.cend(),
                 std::back_inserter(states),
                 [](auto &kv) { return std::make_pair(kv.first, kv.second); });
  return states;
}

void AMD::PMFVVoltCurve::voltCurvePoint(unsigned int pointIndex,
                                        units::frequency::megahertz_t freq,
                                        units::voltage::millivolt_t volt)
{
  if (pointIndex < voltCurve_.size()) {
    auto [pointFreqRange, pointVoltRange] = voltRange_.at(pointIndex);
    auto &[pointFreq, pointVolt] = voltCurve_.at(pointIndex);
    pointFreq = std::clamp(freq, pointFreqRange.first, pointFreqRange.second);
    pointVolt = std::clamp(volt, pointVoltRange.first, pointVoltRange.second);
  }
}

void AMD::PMFVVoltCurve::gpuState(unsigned int index,
                                  units::frequency::megahertz_t freq)
{
  auto [gpuMin, gpuMax] = gpuRange();
  auto &sFreq = gpuStates_.at(index);
  sFreq = std::clamp(freq, gpuMin, gpuMax);
}

void AMD::PMFVVoltCurve::memState(unsigned int index,
                                  units::frequency::megahertz_t freq)
{
  auto [memMin, memMax] = memRange();
  auto &sFreq = memStates_.at(index);
  sFreq = std::clamp(freq, memMin, memMax);
}

std::string
AMD::PMFVVoltCurve::ppOdClkVoltStateCmd(std::string &&type, unsigned int index,
                                        units::frequency::megahertz_t freq) const
{
  return type.append(" ")
      .append(std::to_string(index))
      .append(" ")
      .append(std::to_string(freq.to<int>()));
}

std::string
AMD::PMFVVoltCurve::ppOdClkVoltCurveCmd(unsigned int pointIndex,
                                        units::frequency::megahertz_t freq,
                                        units::voltage::millivolt_t volt) const
{
  return std::string("vc ")
      .append(std::to_string(pointIndex))
      .append(" ")
      .append(std::to_string(freq.to<unsigned int>()))
      .append(" ")
      .append(std::to_string(volt.to<unsigned int>()));
}
