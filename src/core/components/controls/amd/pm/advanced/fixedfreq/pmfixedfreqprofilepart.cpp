// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedfreqprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

class AMD::PMFixedFreqProfilePart::Initializer final
: public PMFixedFreq::Exporter
{
 public:
  Initializer(AMD::PMFixedFreqProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;

  void takePMFixedFreqSclkIndex(unsigned int index) override;
  void takePMFixedFreqMclkIndex(unsigned int index) override;

  void takePMFixedFreqSclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  void takePMFixedFreqMclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFixedFreqProfilePart &outer_;
};

void AMD::PMFixedFreqProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFixedFreqProfilePart::Initializer::takePMFixedFreqSclkIndex(
    unsigned int index)
{
  outer_.sclkIndex_ = index;
}

void AMD::PMFixedFreqProfilePart::Initializer::takePMFixedFreqMclkIndex(
    unsigned int index)
{
  outer_.mclkIndex_ = index;
}

void AMD::PMFixedFreqProfilePart::Initializer::takePMFixedFreqSclkStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.sclkIndices_.reserve(states.size());
  std::transform(states.cbegin(), states.cend(),
                 std::back_inserter(outer_.sclkIndices_),
                 [](auto &kv) { return kv.first; });
}

void AMD::PMFixedFreqProfilePart::Initializer::takePMFixedFreqMclkStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.mclkIndices_.reserve(states.size());
  std::transform(states.cbegin(), states.cend(),
                 std::back_inserter(outer_.mclkIndices_),
                 [](auto &kv) { return kv.first; });
}

AMD::PMFixedFreqProfilePart::PMFixedFreqProfilePart() noexcept
: id_(AMD::PMFixedFreq::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFixedFreqProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFixedFreqProfilePart::initializer()
{
  return std::make_unique<AMD::PMFixedFreqProfilePart::Initializer>(*this);
}

std::string const &AMD::PMFixedFreqProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedFreqProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFixedFreqProfilePart::provideActive() const
{
  return active();
}

unsigned int AMD::PMFixedFreqProfilePart::providePMFixedFreqSclkIndex() const
{
  return sclkIndex_;
}

unsigned int AMD::PMFixedFreqProfilePart::providePMFixedFreqMclkIndex() const
{
  return mclkIndex_;
}

void AMD::PMFixedFreqProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmFreqImporter = dynamic_cast<AMD::PMFixedFreqProfilePart::Importer &>(i);
  sclkIndex(pmFreqImporter.providePMFixedFreqSclkIndex());
  mclkIndex(pmFreqImporter.providePMFixedFreqMclkIndex());
}

void AMD::PMFixedFreqProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmFreqExporter = dynamic_cast<AMD::PMFixedFreqProfilePart::Exporter &>(e);
  pmFreqExporter.takePMFixedFreqSclkIndex(sclkIndex_);
  pmFreqExporter.takePMFixedFreqMclkIndex(mclkIndex_);
}

std::unique_ptr<IProfilePart> AMD::PMFixedFreqProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFixedFreqProfilePart>();

  clone->sclkIndices_ = sclkIndices_;
  clone->mclkIndices_ = mclkIndices_;

  clone->sclkIndex_ = sclkIndex_;
  clone->mclkIndex_ = mclkIndex_;

  return std::move(clone);
}

void AMD::PMFixedFreqProfilePart::sclkIndex(unsigned int index)
{
  clkIndex(sclkIndex_, index, sclkIndices_);
}

void AMD::PMFixedFreqProfilePart::mclkIndex(unsigned int index)
{
  clkIndex(mclkIndex_, index, mclkIndices_);
}

void AMD::PMFixedFreqProfilePart::clkIndex(
    unsigned int &targetIndex, unsigned int newIndex,
    std::vector<unsigned int> const &availableIndices) const
{
  auto indexIt = std::find(availableIndices.cbegin(), availableIndices.cend(),
                           newIndex);
  if (indexIt != availableIndices.cend())
    targetIndex = newIndex;
}

bool const AMD::PMFixedFreqProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFixedFreq::ItemID, []() {
      return std::make_unique<AMD::PMFixedFreqProfilePart>();
    });
