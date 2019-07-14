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
#include "pmfixedfreqprofilepart.h"

#include "core/profilepartprovider.h"

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
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &) override
  {
  }

  void takePMFixedFreqMclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &) override
  {
  }

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
  sclkIndex_ = pmFreqImporter.providePMFixedFreqSclkIndex();
  mclkIndex_ = pmFreqImporter.providePMFixedFreqMclkIndex();
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
  clone->sclkIndex_ = sclkIndex_;
  clone->mclkIndex_ = mclkIndex_;

  return std::move(clone);
}

bool const AMD::PMFixedFreqProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFixedFreq::ItemID, []() {
      return std::make_unique<AMD::PMFixedFreqProfilePart>();
    });
