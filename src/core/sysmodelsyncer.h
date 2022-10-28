// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "commandqueue.h"
#include "helper/ihelpersysctl.h"
#include "iprofileapplicator.h"
#include "isysmodel.h"
#include "isysmodelsyncer.h"
#include <QObject>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

class IProfileView;

class SysModelSyncer final
: public QObject
, public ISysModelSyncer
, public IProfileApplicator
{
  Q_OBJECT

 public:
  SysModelSyncer(std::unique_ptr<ISysModel> &&sysModel,
                 std::unique_ptr<IHelperSysCtl> &&helperSysCtl) noexcept;

  ISysModel &sysModel() const override;
  void settingChanged(QString const &key, QVariant const &value) override;

  void init() override;
  void stop() override;

  void apply(IProfileView &profileView) override;

 private:
  void updateSensors();
  void syncModel();

  std::unique_ptr<ISysModel> const sysModel_;
  std::unique_ptr<IHelperSysCtl> const helperSysCtl_;

  std::mutex syncMutex_;
  CommandQueue cmds_;

  std::mutex sensorsMutex_;
  std::unordered_map<std::string, std::unordered_set<std::string>> ignoredSensors_;

  std::unique_ptr<std::thread> updateThread_;
  std::unique_ptr<std::thread> syncThread_;
  std::atomic<bool> stopSignal_{false};
};
