// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "systeminfoui.h"

#include "app/app.h"
#include "isysmodel.h"
#include "qmlcomponentregistry.h"
#include <QApplication>
#include <QClipboard>
#include <QQmlApplicationEngine>
#include <QtGlobal>
#include <QtQml>
#include <algorithm>
#include <iterator>

char const *const SystemInfoUI::trStrings[] = {
    // XXX add info keys here
    QT_TRANSLATE_NOOP("SystemInfoUI", "kernelv"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "mesav"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "vkapiv"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "glcorev"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "glcompv"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "vendorid"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "deviceid"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "svendorid"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "sdeviceid"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "vendor"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "device"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "sdevice"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "pcislot"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "driver"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "revision"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "memory"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "gputype"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "biosv"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "cpufamily"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "model"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "modname"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "stepping"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "ucodev"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "l3cache"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "exeunits"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "cores"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "flags"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "bugs"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "bogomips"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "arch"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "opmode"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "byteorder"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "virt"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "l1dcache"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "l1icache"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "l2cache"),
    QT_TRANSLATE_NOOP("SystemInfoUI", "uniqueid"),
};

SystemInfoUI::SystemInfoUI(QObject *parent) noexcept
: QObject(parent)
{
}

void SystemInfoUI::init(ISysModel const *sysModel)
{
  sysModel_ = sysModel;
  initInfo();

  for (auto &sysModelInfo : info_) {

    QVariantList list;
    for (auto &sysComponentInfo : sysModelInfo.second) {
      list.append(sysComponentInfo.first);
      list.append(sysComponentInfo.second);
    }
    emit addSystemInfo(sysModelInfo.first, list);
  }
}

void SystemInfoUI::copyToClipboard() const
{
  auto clipboard = QApplication::clipboard();
  if (clipboard != nullptr) {
    QString text(App::Name.data());
    text += " v";
    text += App::VersionStr.data();
    text += "\n";

    for (auto &sysModelInfo : info_) {

      auto sectionTitle = sysModelInfo.first;
      sectionTitle.replace("\n", " ");
      text += "\n==== " + sectionTitle + " ====\n";

      for (auto &sysComponentInfo : sysModelInfo.second)
        text += sysComponentInfo.first + ": " + sysComponentInfo.second + "\n";
    }

    clipboard->setText(text);
  }
}

void SystemInfoUI::initInfo()
{
  auto rawInfo = sysModel_->info();

  for (auto &sysModelInfo : rawInfo) {
    std::vector<std::pair<QString, QString>> processedModelInfo;

    // translate keys
    std::transform(sysModelInfo.second.cbegin(), sysModelInfo.second.cend(),
                   std::back_inserter(processedModelInfo), [](auto &pair) {
                     return std::pair(tr(pair.first.c_str()),
                                      QString::fromStdString(pair.second));
                   });

    // sort information elements using the translated keys
    std::sort(processedModelInfo.begin(), processedModelInfo.end(),
              [](auto &left, auto &right) { return left.first < right.first; });

    info_.emplace_back(QString::fromStdString(sysModelInfo.first),
                       std::move(processedModelInfo));
  }
}

bool const SystemInfoUI::registered_ =
    QMLComponentRegistry::addQMLTypeRegisterer([]() {
      qmlRegisterType<SystemInfoUI>("CoreCtrl.UIComponents", 1, 0,
                                    SystemInfoUI::QMLComponentID.data());
    });
