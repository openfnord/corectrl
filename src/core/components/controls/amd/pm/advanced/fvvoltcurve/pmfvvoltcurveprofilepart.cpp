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
#include "pmfvvoltcurveprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>

class AMD::PMFVVoltCurveProfilePart::Initializer final
: public AMD::PMFVVoltCurve::Exporter
{
 public:
  Initializer(AMD::PMFVVoltCurveProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takePMFVVoltCurveVoltModes(std::vector<std::string> const &modes) override;
  void takePMFVVoltCurveVoltRange(
      std::vector<std::pair<
          std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
          std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
          &pointRanges) override;

  void takePMFVVoltCurveGPURange(units::frequency::megahertz_t min,
                                 units::frequency::megahertz_t max) override;
  void takePMFVVoltCurveMemRange(units::frequency::megahertz_t min,
                                 units::frequency::megahertz_t max) override;

  void takeActive(bool active) override;
  void takePMFVVoltCurveVoltMode(std::string const &mode) override;
  void takePMFVVoltCurveVoltCurve(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &curve) override;
  void takePMFVVoltCurveGPUStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  void takePMFVVoltCurveMemStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFVVoltCurveProfilePart &outer_;
};

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveVoltModes(
    std::vector<std::string> const &modes)
{
  outer_.voltModes_ = modes;
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveVoltRange(
    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
        &pointRanges)
{
  outer_.voltCurveRange_ = pointRanges;
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveGPURange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.gpuFreqRange_ = std::make_pair(min, max);
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveMemRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.memFreqRange_ = std::make_pair(min, max);
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveVoltMode(
    std::string const &mode)
{
  outer_.voltMode_ = mode;
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveVoltCurve(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &curve)
{
  outer_.voltCurve_ = curve;
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveGPUStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.gpuStates_ = states;
}

void AMD::PMFVVoltCurveProfilePart::Initializer::takePMFVVoltCurveMemStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.memStates_ = states;
}

AMD::PMFVVoltCurveProfilePart::PMFVVoltCurveProfilePart() noexcept
: id_(AMD::PMFVVoltCurve::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFVVoltCurveProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFVVoltCurveProfilePart::initializer()
{
  return std::make_unique<AMD::PMFVVoltCurveProfilePart::Initializer>(*this);
}

std::string const &AMD::PMFVVoltCurveProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFVVoltCurveProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFVVoltCurveProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMFVVoltCurveProfilePart::providePMFVVoltCurveVoltMode() const
{
  return voltMode_;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVVoltCurveProfilePart::providePMFVVoltCurveVoltCurvePoint(
    unsigned int index) const
{
  if (index < voltCurve_.size())
    return voltCurve_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

units::frequency::megahertz_t
AMD::PMFVVoltCurveProfilePart::providePMFVVoltCurveGPUState(unsigned int index) const
{
  auto stateIt = std::find_if(gpuStates_.cbegin(), gpuStates_.cend(),
                              [=](auto &state) { return state.first == index; });

  if (stateIt != gpuStates_.cend())
    return stateIt->second;
  else
    return units::frequency::megahertz_t(0);
}

units::frequency::megahertz_t
AMD::PMFVVoltCurveProfilePart::providePMFVVoltCurveMemState(unsigned int index) const
{
  auto stateIt = std::find_if(memStates_.cbegin(), memStates_.cend(),
                              [=](auto &state) { return state.first == index; });

  if (stateIt != memStates_.cend())
    return stateIt->second;
  else
    return units::frequency::megahertz_t(0);
}

void AMD::PMFVVoltCurveProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmFVVoltCurveImporter =
      dynamic_cast<AMD::PMFVVoltCurveProfilePart::Importer &>(i);

  voltMode(pmFVVoltCurveImporter.providePMFVVoltCurveVoltMode());

  for (size_t i = 0; i < voltCurve_.size(); ++i) {
    auto [freq, volt] = pmFVVoltCurveImporter.providePMFVVoltCurveVoltCurvePoint(
        static_cast<unsigned int>(i));
    voltCurvePoint(static_cast<unsigned int>(i), freq, volt);
  }

  for (auto [index, _] : gpuStates_)
    gpuState(index, pmFVVoltCurveImporter.providePMFVVoltCurveGPUState(index));

  for (auto [index, _] : memStates_)
    memState(index, pmFVVoltCurveImporter.providePMFVVoltCurveMemState(index));
}

void AMD::PMFVVoltCurveProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmFVVoltCurveExporter =
      dynamic_cast<AMD::PMFVVoltCurveProfilePart::Exporter &>(e);

  pmFVVoltCurveExporter.takePMFVVoltCurveVoltMode(voltMode_);
  pmFVVoltCurveExporter.takePMFVVoltCurveVoltCurve(voltCurve_);
  pmFVVoltCurveExporter.takePMFVVoltCurveGPUStates(gpuStates_);
  pmFVVoltCurveExporter.takePMFVVoltCurveMemStates(memStates_);
}

std::unique_ptr<IProfilePart> AMD::PMFVVoltCurveProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFVVoltCurveProfilePart>();
  clone->voltModes_ = voltModes_;
  clone->voltMode_ = voltMode_;

  clone->voltCurveRange_ = voltCurveRange_;
  clone->voltCurve_ = voltCurve_;

  clone->gpuFreqRange_ = gpuFreqRange_;
  clone->memFreqRange_ = memFreqRange_;

  clone->gpuStates_ = gpuStates_;
  clone->memStates_ = memStates_;

  return std::move(clone);
}

void AMD::PMFVVoltCurveProfilePart::voltMode(std::string const &mode)
{
  auto iter = std::find_if(
      voltModes_.cbegin(), voltModes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != voltModes_.cend())
    voltMode_ = mode;
}

void AMD::PMFVVoltCurveProfilePart::voltCurvePoint(
    unsigned int pointIndex, units::frequency::megahertz_t freq,
    units::voltage::millivolt_t volt)
{
  if (pointIndex < voltCurve_.size()) {
    auto [pointFreqRange, pointVoltRange] = voltCurveRange_.at(pointIndex);
    auto &[pointFreq, pointVolt] = voltCurve_.at(pointIndex);
    pointFreq = std::clamp(freq, pointFreqRange.first, pointFreqRange.second);
    pointVolt = std::clamp(volt, pointVoltRange.first, pointVoltRange.second);
  }
}

void AMD::PMFVVoltCurveProfilePart::gpuState(unsigned int index,
                                             units::frequency::megahertz_t freq)
{
  freqState(index, freq, gpuStates_, gpuFreqRange_);
}

void AMD::PMFVVoltCurveProfilePart::memState(unsigned int index,
                                             units::frequency::megahertz_t freq)
{
  freqState(index, freq, memStates_, memFreqRange_);
}

void AMD::PMFVVoltCurveProfilePart::freqState(
    unsigned int index, units::frequency::megahertz_t freq,
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> &targetStates,
    std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> const
        &targetRange) const
{
  auto stateIt = std::find_if(targetStates.begin(), targetStates.end(),
                              [=](auto &state) { return state.first == index; });
  if (stateIt != targetStates.end())
    stateIt->second = std::clamp(freq, targetRange.first, targetRange.second);
}

bool const AMD::PMFVVoltCurveProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFVVoltCurve::ItemID, []() {
      return std::make_unique<AMD::PMFVVoltCurveProfilePart>();
    });
