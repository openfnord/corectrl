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
#include "cpufreqprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>
#include <utility>

class CPUFreqProfilePart::Initializer final : public CPUFreq::Exporter
{
 public:
  Initializer(CPUFreqProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takeCPUFreqScalingGovernor(std::string const &governor) override;

  void
  takeCPUFreqScalingGovernors(std::vector<std::string> const &governors) override;

 private:
  CPUFreqProfilePart &outer_;
};

void CPUFreqProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void CPUFreqProfilePart::Initializer::takeCPUFreqScalingGovernor(
    std::string const &governor)
{
  outer_.governor_ = governor;
}

void CPUFreqProfilePart::Initializer::takeCPUFreqScalingGovernors(
    std::vector<std::string> const &governors)
{
  outer_.governors_ = governors;
}

CPUFreqProfilePart::CPUFreqProfilePart() noexcept
: id_(CPUFreq::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
CPUFreqProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> CPUFreqProfilePart::initializer()
{
  return std::make_unique<CPUFreqProfilePart::Initializer>(*this);
}

std::string const &CPUFreqProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
CPUFreqProfilePart::provideImporter(Item const &)
{
  return {};
}

bool CPUFreqProfilePart::provideActive() const
{
  return active();
}

std::string const &CPUFreqProfilePart::provideCPUFreqScalingGovernor() const
{
  return governor_;
}

void CPUFreqProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &cpuFreqImporter = dynamic_cast<CPUFreqProfilePart::Importer &>(i);
  governor(cpuFreqImporter.provideCPUFreqScalingGovernor());
}

void CPUFreqProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &cpuFreqExporter = dynamic_cast<CPUFreqProfilePart::Exporter &>(e);
  cpuFreqExporter.takeCPUFreqScalingGovernor(governor_);
}

std::unique_ptr<IProfilePart> CPUFreqProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<CPUFreqProfilePart>();
  clone->governors_ = governors_;
  clone->governor_ = governor_;

  return std::move(clone);
}

void CPUFreqProfilePart::governor(std::string const &governor)
{
  // only import known governors
  auto iter = std::find_if(
      governors_.cbegin(), governors_.cend(),
      [&](auto &availableGovernor) { return governor == availableGovernor; });
  if (iter != governors_.cend())
    governor_ = governor;
}

bool const CPUFreqProfilePart::registered_ = ProfilePartProvider::registerProvider(
    CPUFreq::ItemID, []() { return std::make_unique<CPUFreqProfilePart>(); });
