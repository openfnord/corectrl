// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "helperids.h"
#include <QDBusAbstractAdaptor>
#include <QObject>

class QDBusMessage;

class HelperKiller final : public QDBusAbstractAdaptor
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", DBUS_HELPER_KILLER_INTERFACE)
 public:
  HelperKiller(QObject *parent) noexcept;
  ~HelperKiller();

 public slots: // D-Bus interface slots
  bool start(QDBusMessage const &message) const;

 private:
  bool isAuthorized(QDBusMessage const &message) const;
};
