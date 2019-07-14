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
#include <QThread>

SysModelSyncer::SysModelSyncer(std::unique_ptr<ISysModel> &&sysModel,
                               std::unique_ptr<IHelperSysCtl> &&helperSysCtl) noexcept
: sysModel_(std::move(sysModel))
, helperSysCtl_(std::move(helperSysCtl))
{
  timer_.setInterval(500);
  connect(&timer_, &QTimer::timeout, this, &SysModelSyncer::sync);
}

ISysModel &SysModelSyncer::sysModel() const
{
  return *sysModel_;
}

void SysModelSyncer::init()
{
  helperSysCtl_->init();

  sysModel_->preInit(cmds_);
  helperSysCtl_->apply(cmds_);

  // NOTE give some time to the helper so it applies the pre-init commands
  // before the model initialization.
  QThread::msleep(500);

  sysModel_->init();
  timer_.start();
}

void SysModelSyncer::stop()
{
  timer_.stop();
}

void SysModelSyncer::apply(IProfileView &profileView)
{
  std::lock_guard<std::mutex> lock(mutex_);
  sysModel_->importWith(profileView);
}

void SysModelSyncer::sync()
{
  std::lock_guard<std::mutex> lock(mutex_);
  sysModel_->sync(cmds_);
  helperSysCtl_->apply(cmds_);
}
