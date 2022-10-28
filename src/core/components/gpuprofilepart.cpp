// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuprofilepart.h"

#include "core/info/igpuinfo.h"
#include "core/profilepart.h"
#include "core/profilepartprovider.h"
#include "sensors/isensor.h"
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <utility>

class GPUProfilePart::Factory final
: public ProfilePart::Factory
, public IGPU::Exporter
{
 public:
  Factory(IProfilePartProvider const &profilePartProvider,
          GPUProfilePart &outer) noexcept
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

  void takeInfo(IGPUInfo const &) override;
  void takeSensor(ISensor const &) override;

 private:
  GPUProfilePart &outer_;
};

void GPUProfilePart::Factory::takeProfilePart(std::unique_ptr<IProfilePart> &&part)
{
  outer_.parts_.emplace_back(std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUProfilePart::Factory::provideExporter(Item const &i)
{
  return factory(i.ID());
}

void GPUProfilePart::Factory::takeInfo(IGPUInfo const &info)
{
  // NOTE info and system component key must be initialized here
  outer_.deviceID_ = info.info(IGPUInfo::Keys::deviceID);
  outer_.revision_ = info.info(IGPUInfo::Keys::revision);

  auto uniqueID = info.info(IGPUInfo::Keys::uniqueID);
  if (!uniqueID.empty())
    outer_.uniqueID_ = uniqueID;

  outer_.index_ = info.index();
  outer_.updateKey();
}

void GPUProfilePart::Factory::takeSensor(ISensor const &sensor)
{
  auto sensorProfilePart = createPart(sensor.ID());
  if (sensorProfilePart != nullptr)
    outer_.parts_.emplace_back(std::move(sensorProfilePart));
}

class GPUProfilePart::Initializer final : public IGPU::Exporter
{
 public:
  Initializer(GPUProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;

  void takeInfo(IGPUInfo const &) override
  {
  }

  void takeSensor(ISensor const &) override
  {
  }

 private:
  GPUProfilePart &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUProfilePart::Initializer::provideExporter(Item const &i)
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

void GPUProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

GPUProfilePart::GPUProfilePart() noexcept
: id_(IGPU::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
GPUProfilePart::factory(IProfilePartProvider const &profilePartProvider)
{
  return std::make_unique<GPUProfilePart::Factory>(profilePartProvider, *this);
}

std::unique_ptr<Exportable::Exporter> GPUProfilePart::initializer()
{
  return std::make_unique<GPUProfilePart::Initializer>(*this);
}

std::string const &GPUProfilePart::ID() const
{
  return id_;
}

bool GPUProfilePart::belongsTo(Item const &i) const
{
  auto gpu = dynamic_cast<IGPU const *>(&i);
  if (gpu != nullptr) {
    auto &info = gpu->info();

    // when available, use the GPU unique ID
    auto uniqueID = info.info(IGPUInfo::Keys::uniqueID);
    if (!uniqueID.empty())
      return uniqueID == uniqueID_;

    return info.index() == index_ &&
           info.info(IGPUInfo::Keys::deviceID) == deviceID_ &&
           info.info(IGPUInfo::Keys::revision) == revision_;
  }

  return false;
}

std::string const &GPUProfilePart::key() const
{
  return key_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
GPUProfilePart::provideImporter(Item const &i)
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

bool GPUProfilePart::provideActive() const
{
  return active();
}

void GPUProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  int oldIndex = index_;

  auto &gImporter = dynamic_cast<IGPUProfilePart::Importer &>(i);
  index_ = gImporter.provideIndex();
  deviceID_ = gImporter.provideDeviceID();
  revision_ = gImporter.provideRevision();
  uniqueID_ = gImporter.provideUniqueID();

  if (oldIndex != index_)
    updateKey();

  for (auto &part : parts_)
    part->importWith(i);
}

void GPUProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &gExporter = dynamic_cast<IGPUProfilePart::Exporter &>(e);
  gExporter.takeIndex(index_);
  gExporter.takeDeviceID(deviceID_);
  gExporter.takeRevision(revision_);
  gExporter.takeUniqueID(uniqueID_);

  for (auto &part : parts_)
    part->exportWith(e);
}

std::unique_ptr<IProfilePart> GPUProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<GPUProfilePart>();
  clone->deviceID_ = deviceID_;
  clone->revision_ = revision_;
  clone->uniqueID_ = uniqueID_;
  clone->index_ = index_;
  clone->key_ = key_;
  clone->parts_.reserve(parts_.size());
  std::transform(parts_.cbegin(), parts_.cend(),
                 std::back_inserter(clone->parts_),
                 [](auto &part) { return part->clone(); });

  return std::move(clone);
}

void GPUProfilePart::updateKey()
{
  key_ = "GPU" + std::to_string(index_);
}

bool const GPUProfilePart::registered_ = ProfilePartProvider::registerProvider(
    IGPU::ItemID, []() { return std::make_unique<GPUProfilePart>(); });
