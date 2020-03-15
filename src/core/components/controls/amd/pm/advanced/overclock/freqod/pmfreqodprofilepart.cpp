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
#include "pmfreqodprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>
#include <utility>

class AMD::PMFreqOdProfilePart::Initializer final : public PMFreqOd::Exporter
{
 public:
  Initializer(AMD::PMFreqOdProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqOdBaseSclk(units::frequency::megahertz_t) override
  {
  }

  void takePMFreqOdBaseMclk(units::frequency::megahertz_t) override
  {
  }

  void takePMFreqOdSclkOd(unsigned int value) override;
  void takePMFreqOdMclkOd(unsigned int value) override;

 private:
  AMD::PMFreqOdProfilePart &outer_;
};

void AMD::PMFreqOdProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMFreqOdProfilePart::Initializer::takePMFreqOdSclkOd(unsigned int value)
{
  outer_.sclkOd_ = value;
}

void AMD::PMFreqOdProfilePart::Initializer::takePMFreqOdMclkOd(unsigned int value)
{
  outer_.mclkOd_ = value;
}

AMD::PMFreqOdProfilePart::PMFreqOdProfilePart() noexcept
: id_(AMD::PMFreqOd::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqOdProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqOdProfilePart::initializer()
{
  return std::make_unique<AMD::PMFreqOdProfilePart::Initializer>(*this);
}

std::string const &AMD::PMFreqOdProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqOdProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMFreqOdProfilePart::provideActive() const
{
  return active();
}

unsigned int AMD::PMFreqOdProfilePart::providePMFreqOdSclkOd() const
{
  return sclkOd_;
}

unsigned int AMD::PMFreqOdProfilePart::providePMFreqOdMclkOd() const
{
  return mclkOd_;
}

void AMD::PMFreqOdProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmFreqImporter = dynamic_cast<AMD::PMFreqOdProfilePart::Importer &>(i);
  sclkOd(pmFreqImporter.providePMFreqOdSclkOd());
  mclkOd(pmFreqImporter.providePMFreqOdMclkOd());
}

void AMD::PMFreqOdProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmFreqExporter = dynamic_cast<AMD::PMFreqOdProfilePart::Exporter &>(e);
  pmFreqExporter.takePMFreqOdSclkOd(sclkOd_);
  pmFreqExporter.takePMFreqOdMclkOd(mclkOd_);
}

std::unique_ptr<IProfilePart> AMD::PMFreqOdProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMFreqOdProfilePart>();
  clone->sclkOd_ = sclkOd_;
  clone->mclkOd_ = mclkOd_;

  return std::move(clone);
}

void AMD::PMFreqOdProfilePart::sclkOd(unsigned int value)
{
  sclkOd_ = std::clamp(value, 0u, 20u);
}

void AMD::PMFreqOdProfilePart::mclkOd(unsigned int value)
{
  mclkOd_ = std::clamp(value, 0u, 20u);
}

bool const AMD::PMFreqOdProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMFreqOd::ItemID, []() {
      return std::make_unique<AMD::PMFreqOdProfilePart>();
    });
