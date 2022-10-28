// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

// clang-format off
#define POLKIT_HELPER_KILLER_ACTION   "org.corectrl.helperkiller.init"
#define DBUS_HELPER_KILLER_SERVICE    "org.corectrl.helperkiller"
#define DBUS_HELPER_KILLER_INTERFACE  "org.corectrl.helperkiller"
#define DBUS_HELPER_KILLER_PATH       "/"

#define POLKIT_HELPER_ACTION          "org.corectrl.helper.init"
#define DBUS_HELPER_SERVICE           "org.corectrl.helper"
#define DBUS_HELPER_INTERFACE         "org.corectrl.helper"
#define DBUS_HELPER_PATH              "/Helper"
#define DBUS_HELPER_PMON_INTERFACE    "org.corectrl.helper.pmon"
#define DBUS_HELPER_PMON_PATH         "/Helper/PMon"
#define DBUS_HELPER_SYSCTL_INTERFACE  "org.corectrl.helper.sysctl"
#define DBUS_HELPER_SYSCTL_PATH       "/Helper/SysCtl"

#define LOG_HELPER_FILE_NAME          "CoreCtrl_helper.log"
#define HELPER_EXE                    "corectrl_helper"
// clang-format on
