// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqvoltprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <iterator>
#include <memory>

class AMD::PMFreqVoltProfilePart::Initializer final
: public AMD::PMFreqVolt::Exporter
{
 public:
  Initializer(AMD::PMFreqVoltProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqVoltControlName(std::string const &name) override;
  void takePMFreqVoltVoltModes(std::vector<std::string> const &modes) override;
  void takePMFreqVoltVoltRange(units::voltage::millivolt_t min,
                               units::voltage::millivolt_t max) override;
  void takePMFreqVoltFreqRange(units::frequency::megahertz_t min,
                               units::frequency::megahertz_t max) override;
  void takePMFreqVoltVoltMode(std::string const &mode) override;
  void takePMFreqVoltStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFreqVoltActiveStates(std::vector<unsigned int> const &states) override;

 private:
  AMD::PMFreqVoltProfilePart &outer_;
};

void AMD::PMFreqVoltProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltControlName(
    std::string const &name)
{
  outer_.controlName_ = name;
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltVoltModes(
    std::vector<std::string> const &modes)
{
  outer_.voltModes_ = modes;
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltVoltRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  outer_.voltRange_ = std::make_pair(min, max);
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltFreqRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.freqRange_ = std::make_pair(min, max);
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltVoltMode(
    std::string const &mode)
{
  outer_.voltMode_ = mode;
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.states_ = states;
}

void AMD::PMFreqVoltProfilePart::Initializer::takePMFreqVoltActiveStates(
    std::vector<unsigned int> const &states)
{
  outer_.activeStates_ = states;
}

AMD::PMFreqVoltProfilePart::PMFreqVoltProfilePart() noexcept
: id_(AMD::PMFreqVolt::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqVoltProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqVoltProfilePart::initializer()
{
  return std::make_unique<AMD::PMFreqVoltProfilePart::Initializer>(*this);
}

std::string const &AMD::PMFreqVoltProfilePart::ID() const
{
  return id_;
}

std::string const &AMD::PMFreqVoltProfilePart::instanceID() const
{
  return controlName_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqVoltProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFreqVoltProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMFreqVoltProfilePart::providePMFreqVoltVoltMode() const
{
  return voltMode_;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFreqVoltProfilePart::providePMFreqVoltState(unsigned int index) const
{
  auto stateIt = std::find_if(states_.cbegin(), states_.cend(), [=](auto &state) {
    return std::get<0>(state) == index;
  });

  if (stateIt != states_.cend())
    return std::make_pair(std::get<1>(*stateIt), std::get<2>(*stateIt));
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

std::vector<unsigned int>
AMD::PMFreqVoltProfilePart::providePMFreqVoltActiveStates() const
{
  return activeStates_;
}

void AMD::PMFreqVoltProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMFreqVoltProfilePart::Importer &>(i);

  voltMode(importer.providePMFreqVoltVoltMode());

  for (auto &[index, _1, _2] : states_)
    state(index, importer.providePMFreqVoltState(index));

  activateStates(importer.providePMFreqVoltActiveStates());
}

void AMD::PMFreqVoltProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &exporter = dynamic_cast<AMD::PMFreqVoltProfilePart::Exporter &>(e);

  exporter.takePMFreqVoltControlName(controlName_);
  exporter.takePMFreqVoltVoltMode(voltMode_);
  exporter.takePMFreqVoltStates(states_);
  exporter.takePMFreqVoltActiveStates(activeStates_);
}

std::unique_ptr<IProfilePart> AMD::PMFreqVoltProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFreqVoltProfilePart>();

  clone->controlName_ = controlName_;
  clone->voltModes_ = voltModes_;
  clone->voltMode_ = voltMode_;
  clone->voltRange_ = voltRange_;
  clone->freqRange_ = freqRange_;
  clone->states_ = states_;
  clone->activeStates_ = activeStates_;

  return std::move(clone);
}

void AMD::PMFreqVoltProfilePart::voltMode(std::string const &mode)
{
  auto iter = std::find_if(
      voltModes_.cbegin(), voltModes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });

  if (iter != voltModes_.cend())
    voltMode_ = mode;
}

void AMD::PMFreqVoltProfilePart::state(
    unsigned int index,
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t> const &value)
{
  auto stateIt = std::find_if(states_.begin(), states_.end(), [=](auto &state) {
    return std::get<0>(state) == index;
  });

  if (stateIt != states_.end()) {
    auto &[_, sFreq, sVolt] = *stateIt;
    sFreq = std::clamp(value.first, freqRange_.first, freqRange_.second);
    sVolt = std::clamp(value.second, voltRange_.first, voltRange_.second);
  }
}

void AMD::PMFreqVoltProfilePart::activateStates(
    std::vector<unsigned int> const &states)
{
  std::vector<unsigned int> active;
  std::copy_if(states.cbegin(), states.cend(), std::back_inserter(active),
               [&](unsigned int index) {
                 // skip unknown state states
                 return std::find_if(states_.cbegin(), states_.cend(),
                                     [&](auto &state) {
                                       return std::get<0>(state) == index;
                                     }) != states_.cend();
               });
  if (!active.empty()) // at least one state must be active
    std::swap(active, activeStates_);
}

bool const AMD::PMFreqVoltProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFreqVolt::ItemID, []() {
      return std::make_unique<AMD::PMFreqVoltProfilePart>();
    });
