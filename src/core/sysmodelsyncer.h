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
