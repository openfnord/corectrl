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
#include "cpuprofilepart.h"

#include "core/info/icpuinfo.h"
#include "core/profilepart.h"
#include "core/profilepartprovider.h"
#include "sensors/isensor.h"
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

class CPUProfilePart::Factory final
: public ProfilePart::Factory
, public ICPU::Exporter
{
 public:
  Factory(IProfilePartProvider const &profilePartProvider,
          CPUProfilePart &outer) noexcept
  : ProfilePart::Factory(profilePartProvider)
  , outer_(outer)
  {
  }

  void takeProfilePart(std::unique_ptr<IProfilePart> &&part) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override
  {
  }

  void takeInfo(ICPUInfo const &) override;
  void takeSensor(ISensor const &) override;

 private:
  CPUProfilePart &outer_;
};

void CPUProfilePart::Factory::takeProfilePart(std::unique_ptr<IProfilePart> &&part)
{
  outer_.parts_.emplace_back(std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUProfilePart::Factory::provideExporter(Item const &i)
{
  return factory(i.ID());
}

void CPUProfilePart::Factory::takeInfo(ICPUInfo const &info)
{
  // NOTE info and system component key must be initialized here
  outer_.socketId_ = info.socketId();
  outer_.updateKey();
}

void CPUProfilePart::Factory::takeSensor(ISensor const &sensor)
{
  auto sensorProfilePart = createPart(sensor.ID());
  if (sensorProfilePart != nullptr)
    outer_.parts_.emplace_back(std::move(sensorProfilePart));
}

class CPUProfilePart::Initializer final : public ICPU::Exporter
{
 public:
  Initializer(CPUProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;

  void takeInfo(ICPUInfo const &) override
  {
  }

  void takeSensor(ISensor const &) override
  {
  }

 private:
  CPUProfilePart &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUProfilePart::Initializer::provideExporter(Item const &i)
{
  for (auto &part : outer_.parts_) {
    auto &id = part->ID();
    if (id == i.ID()) {
      if (initializers_.count(id) > 0)
        return *initializers_.at(id);
      else {
        auto initializer = part->initializer();
        if (initializer != nullptr) {
          initializers_.emplace(id, std::move(initializer));
          return *initializers_.at(id);
        }
      }

      break;
    }
  }

  return {};
}

void CPUProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

CPUProfilePart::CPUProfilePart() noexcept
: id_(ICPU::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
CPUProfilePart::factory(IProfilePartProvider const &profilePartProvider)
{
  return std::make_unique<CPUProfilePart::Factory>(profilePartProvider, *this);
}

std::unique_ptr<Exportable::Exporter> CPUProfilePart::initializer()
{
  return std::make_unique<CPUProfilePart::Initializer>(*this);
}

std::string const &CPUProfilePart::ID() const
{
  return id_;
}

bool CPUProfilePart::belongsTo(Item const &i) const
{
  auto cpu = dynamic_cast<ICPU const *>(&i);
  if (cpu != nullptr)
    return cpu->info().socketId() == socketId_;

  return false;
}

std::string const &CPUProfilePart::key() const
{
  return key_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
CPUProfilePart::provideImporter(Item const &i)
{
  auto &id = i.ID();
  auto partIter = std::find_if(parts_.cbegin(), parts_.cend(),
                               [&](auto &part) { return part->ID() == id; });

  if (partIter != parts_.cend()) {
    auto importer = dynamic_cast<Importable::Importer *>(partIter->get());
    if (importer != nullptr)
      return *importer;
  }

  return {};
}

bool CPUProfilePart::provideActive() const
{
  return active();
}

void CPUProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  int oldSocketId = socketId_;

  auto &partImporter = dynamic_cast<ICPUProfilePart::Importer &>(i);
  socketId_ = partImporter.provideSocketId();

  if (oldSocketId != socketId_)
    updateKey();

  for (auto &part : parts_)
    part->importWith(i);
}

void CPUProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &partExporter = dynamic_cast<ICPUProfilePart::Exporter &>(e);
  partExporter.takeSocketId(socketId_);

  for (auto &part : parts_)
    part->exportWith(e);
}

std::unique_ptr<IProfilePart> CPUProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<CPUProfilePart>();
  clone->socketId_ = socketId_;
  clone->key_ = key_;
  clone->parts_.reserve(parts_.size());
  std::transform(parts_.cbegin(), parts_.cend(),
                 std::back_inserter(clone->parts_),
                 [](auto &part) { return part->clone(); });

  return std::move(clone);
}

void CPUProfilePart::updateKey()
{
  key_ = "CPU" + std::to_string(socketId_);
}

bool const CPUProfilePart::registered_ = ProfilePartProvider::registerProvider(
    ICPU::ItemID, []() { return std::make_unique<CPUProfilePart>(); });
