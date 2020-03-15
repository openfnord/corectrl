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
#include "sysmodelsyncer.h"

#include "iprofileview.h"
#include <QStringList>
#include <QVariant>
#include <chrono>
#include <utility>

SysModelSyncer::SysModelSyncer(std::unique_ptr<ISysModel> &&sysModel,
                               std::unique_ptr<IHelperSysCtl> &&helperSysCtl) noexcept
: sysModel_(std::move(sysModel))
, helperSysCtl_(std::move(helperSysCtl))
{
}

ISysModel &SysModelSyncer::sysModel() const
{
  return *sysModel_;
}

void SysModelSyncer::settingChanged(QString const &key, QVariant const &value)
{
  if (key == "Workarounds/ignoredSensors") {
    std::lock_guard<std::mutex> lock(sensorsMutex_);
    ignoredSensors_.clear();

    auto sensorList = value.toStringList();
    for (auto &sensor : sensorList) {
      auto componentSensorIdList = sensor.split('/');
      if (componentSensorIdList.size() == 2) {
        auto component = componentSensorIdList.at(0).toStdString();
        auto sensorId = componentSensorIdList.at(1).toStdString();

        if (ignoredSensors_.count(component) == 0)
          ignoredSensors_[component] = {};

        ignoredSensors_[component].emplace(sensorId);
      }
    }
  }
}

void SysModelSyncer::init()
{
  helperSysCtl_->init();

  sysModel_->preInit(cmds_);
  helperSysCtl_->apply(cmds_);

  // NOTE give some time to the helper so it applies the pre-init commands
  // before the model initialization.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  sysModel_->init();

  sysModel_->postInit(cmds_);
  helperSysCtl_->apply(cmds_);

  // start the sensor updating thread
  updateThread_ = std::make_unique<std::thread>([&]() {
    while (!stopSignal_.load(std::memory_order_relaxed)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      updateSensors();
    }
  });

  // start the model syncing thread
  syncThread_ = std::make_unique<std::thread>([&]() {
    while (!stopSignal_.load(std::memory_order_relaxed)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      syncModel();
    }
  });
}

void SysModelSyncer::stop()
{
  stopSignal_.store(true, std::memory_order_relaxed);
  updateThread_->join();
  syncThread_->join();
}

void SysModelSyncer::apply(IProfileView &profileView)
{
  std::lock_guard<std::mutex> lock(syncMutex_);
  sysModel_->importWith(profileView);
}

void SysModelSyncer::updateSensors()
{
  std::lock_guard<std::mutex> lock(sensorsMutex_);
  sysModel_->updateSensors(ignoredSensors_);
}

void SysModelSyncer::syncModel()
{
  std::lock_guard<std::mutex> lock(syncMutex_);
  sysModel_->sync(cmds_);
  helperSysCtl_->apply(cmds_);
}
