// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

class ISysModel;
class QString;
class QVariant;

class ISysModelSyncer
{
 public:
  virtual ISysModel &sysModel() const = 0;
  virtual void settingChanged(QString const &key, QVariant const &value) = 0;

  virtual void init() = 0;
  virtual void stop() = 0;

  virtual ~ISysModelSyncer() = default;
};
