// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
