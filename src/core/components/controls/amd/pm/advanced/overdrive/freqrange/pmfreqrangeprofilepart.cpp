// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqrangeprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>

class AMD::PMFreqRangeProfilePart::Initializer final
: public AMD::PMFreqRange::Exporter
{
 public:
  Initializer(AMD::PMFreqRangeProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqRangeControlName(std::string const &name) override;
  void takePMFreqRangeStateRange(units::frequency::megahertz_t min,
                                 units::frequency::megahertz_t max) override;
  void takePMFreqRangeStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFreqRangeProfilePart &outer_;
};

void AMD::PMFreqRangeProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFreqRangeProfilePart::Initializer::takePMFreqRangeControlName(
    std::string const &name)
{
  outer_.controlName_ = name;
}

void AMD::PMFreqRangeProfilePart::Initializer::takePMFreqRangeStateRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.stateRange_ = std::make_pair(min, max);
}

void AMD::PMFreqRangeProfilePart::Initializer::takePMFreqRangeStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.states_ = states;
}

AMD::PMFreqRangeProfilePart::PMFreqRangeProfilePart() noexcept
: id_(AMD::PMFreqRange::ItemID)
{
}

std::string const &AMD::PMFreqRangeProfilePart::ID() const
{
  return id_;
}

std::string const &AMD::PMFreqRangeProfilePart::instanceID() const
{
  return controlName_;
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqRangeProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqRangeProfilePart::initializer()
{
  return std::make_unique<AMD::PMFreqRangeProfilePart::Initializer>(*this);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqRangeProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFreqRangeProfilePart::provideActive() const
{
  return active();
}

units::frequency::megahertz_t
AMD::PMFreqRangeProfilePart::providePMFreqRangeState(unsigned int index) const
{
  auto stateIt = std::find_if(states_.cbegin(), states_.cend(),
                              [=](auto &state) { return state.first == index; });
  if (stateIt != states_.cend())
    return stateIt->second;
  else
    return units::frequency::megahertz_t(0);
}

void AMD::PMFreqRangeProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMFreqRangeProfilePart::Importer &>(i);
  for (auto [index, _] : states_)
    setState(index, importer.providePMFreqRangeState(index));
}

void AMD::PMFreqRangeProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &exporter = dynamic_cast<AMD::PMFreqRangeProfilePart::Exporter &>(e);
  exporter.takePMFreqRangeControlName(controlName_);
  exporter.takePMFreqRangeStates(states_);
}

std::unique_ptr<IProfilePart> AMD::PMFreqRangeProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFreqRangeProfilePart>();
  clone->controlName_ = controlName_;
  clone->stateRange_ = stateRange_;
  clone->states_ = states_;

  return std::move(clone);
}

void AMD::PMFreqRangeProfilePart::setState(unsigned int index,
                                           units::frequency::megahertz_t freq)
{
  auto stateIt = std::find_if(states_.begin(), states_.end(),
                              [=](auto &state) { return state.first == index; });
  if (stateIt != states_.end())
    stateIt->second = std::clamp(freq, stateRange_.first, stateRange_.second);
}

bool const AMD::PMFreqRangeProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFreqRange::ItemID, []() {
      return std::make_unique<AMD::PMFreqRangeProfilePart>();
    });
